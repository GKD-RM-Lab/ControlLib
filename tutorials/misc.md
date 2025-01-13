# MISC （关于CAN/SERIAL设备的一些杂项）

由于我们电控技术路线选择的是`mini-pc`上位机跟`USB_TO_CAN`的方案，由于操作系统默认是根据识别到设备先后顺序给他们分配默认的文件描述符或者网络设备名称。

所以下面的内容基本上是为了避免发生你希望代码里的`can0`是一直控制底盘的，但是由于不可控的上电顺序实际上对应上云台的硬件can设备然后导致疯车惨案

### 硬件
虽然上电顺序无法保证，但是硬件组的同学实现了一种可以通过拨马开关改`USB_TO_CAN`这个usb设备的vid/pid的机制。

e.g :⬇️哨兵大yaw控制板 [@FishSWA](https://github.com/FishSWA)
![alt text](img/sentry_control_board.jpg)

通过`lsusb`指令我们可以看到`GKD Robotics USB2CAN`这个设备的`vid:pid`是
`000d:606f`，他的vid部分的低位可以通过拨动拨码开关调整。
![alt text](img/lsusb.png)

所以与其比起保证上电顺序跟操作系统默认分配的设备名称一直是指定的先上电的设备为can0开始依次递增，我们可以通过vid:pid键值来确定设备名称，这样接线的时候跟上电都不用特别注意，但是相对的要配置的东西也更多。
### udev
所以基于这个机制我们可以在Linux上通过[udev](https://wiki.archlinux.org/title/Udev)给固定的`vid:pid`键值对的USB设备固定一个设备名称。 (想仔细了解udev的可以阅读一下超链接

大家不妨在linux终端里输入
```bash
man bash
```

一般来说了解一个指令最快最准确的方式就是去读他的手册。

UDEV(7)                                                           udev                                                          

NAME
       udev - Dynamic device management

DESCRIPTION
       udev supplies the system software with device events, manages permissions of device nodes and may create additional symlinks in
       the /dev/ directory, or renames network interfaces. **The kernel usually just assigns unpredictable device names based on the order
       of discovery. Meaningful symlinks or network device names provide a way to reliably identify devices based on their properties or
       current configuration.**

udev的手册里十分准确的概括了udev的意义，就是给设备起一个有意义的名字，而不是linux跟据发现他们的顺序分配给他们的默认名字。

e.g: 这个是哨兵mini-pc配置IMU跟CAN_TO_USB名称的udev rules
![alt text](img/iplink.png)

如果udev rules在正常工作那么socket can设备与imu串口设备大概会是这样
![alt text](img/iplink2.png)

置于如何去写一个`udev rules`，我的建议是直接凭直觉照着我们写好的进行关键字替换就好了，如果你真的感兴趣的话-> [how to write udev rules](https://www.reactivated.net/writing_udev_rules.html#example-printer)

### 硬件设备接口自启动
在上面的`udev rules`中可以看到串口设备的最后有 `MODE="0660", GROUP="pkg"`
，这个设置串口设备的全线跟所属用户组的，这样可以在执行电控代码的时候不用`sudo`也可以读串口设备。

但是`socket can`设备通常情况下需要通过
```bash
sudo ip link set can0 up type can bitrate 1000000
```
这条指令来开启

你肯定不希望每辆车在比赛前还要那块屏幕键盘敲半天指令才能拉起来对吧😂

所以一个很自然的想法就是用把所有的这些指令放到一个`.sh`shell脚本里用`systemd`在linux boot之后自动执行一次把车拉起来。但是这个方法不够好，有几个原因:
1. 每辆车can的配置情况不同，舵轮(虽然我们还没有)，哨兵这种需要4 5路can拉起来的车跟英雄步兵这种只需要2 3路就可以拉起来的车有区别，如果你打算把所有的指令用`shell`脚本执行一次大概率你需要每个车一个shell脚本，因为如果把所有可能的`socket can`设备全写进去，`shell`脚本在执行到开启不存在的`socket can`设备的时候会错误退出，后面可能存在的`socket can`设备就不会被初始化
2. 他没法在你的can从松动或者重新插拔之后再执行一次，因为本质山这种配置各脚本让`systemd`然后退出的行为是`systemd`中定义的`oneshot`行为：`Type=oneshot`: this is useful for scripts that do a single job and then exit.他没法在你热插拔之后监测事件再执行一次

事实上我们一开始也一直是这么写的，但是你只能祈祷你的车上电之后一切接线正常并且之后的剧烈运动不会导致接口松动。但是事实是这种事情确实经常发生，这个时候你只能狼狈的在检查完接线之后再去电脑前面一个个看是拿个设备每起来🥹

#### 为了不用每次在启动车的时候开祷😡🤚
其实串口我们是不用担心的，他的权限(`MODE="0660", GOURP="gkd"`)设置跟创建软连接是`udev`做的，而udev是通过监听`linux kernel`的事件触发的，所以如果串口松了重新插拔检查接线之后电控代码就可以准备使用了。

但是对于`socket can`设备需要多配置一些东西
- 如果没开`systemd-networkd`,开一下
```bash
sudo systemctl enable systemd-networkd.service
```
- 编辑`systemd`配置文件,把所有可能会用到的`socket can`设备全写进去
```bash
sudo vim /etc/systemd/network/80-gkd-can.network
```
![](img/gkd-can.png)
- 重新加载`systemd`服务
```bash
sudo systemctl restart systemd-networkd
```

好了，现在你就不用开祷了🤓☝️

### 修改linux kernel gs_usb 驱动

由于`socket can`的内核驱动 `gs_usb`,是根据`vid:pid`去为不一样的`socket can`固件对应的硬件设备加载驱动的，所以我们修改`vid:pid`键值对的方案必须要把所有可能出现的`vid`也加在这里，但是这个不用特别修改，只用使用我们提前编译的内核模块就行（前提是你也是ubuntu22.04

![](img/gs_usb.png)

- 删掉本来的`gs_usb` kernel module
```bash
sudo rm /lib/modules/$(uname -r)/kernel/drivers/net/can/usb/gs_usb.ko.zst
```

- 用改过的.ko替换原本的`gs_usb` kernel module
```bash
 sudo cp /path/to/gs_usb.ko.zst /lib/modules/$(uname -r)/kernel/drivers/net/can/usb/gs_usb.ko.zst
```

### P.S.
如果哪里没按照预期工作，哈哈，那100%是你们自己的问题😃

![](img/mbh.jpg)