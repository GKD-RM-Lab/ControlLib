# 快速启动
首先请确保你会使用 USB to CAN，我们提供了一个[USB to CAN的快速入门教程](USB2CAN.md)

### 启动can总线
你需要引入`io.hpp`，随后你就可以方便地启动can，下面是启动`can0`的例子
```c++
IO::io<CAN>.insert("can0");
```

### 创建一个M3508电机
你需要引入`dji_motor.hpp`，随后你就可以方便地创建电机，下面的例子创建了一个在`can0`上的 id 为 1 的 M3508 电机
```c++
Hardware::DJIMotor motor(3508, "can0", 1);
```
- 三个参数分别是电机类型(3508/6020/2006)、所在的can总线、电机id
- 电机类型设有**编译期的静态检查**，可以放心使用

### 启动电机
使能电机
```c++
motor.enable();
```
开启管理进程
```c++
Hardware::DJIMotorManager::start();
```
随后就可以直接用set接口设置电流了
```c++
motor.set(5000);
```
这个简单的demo位于[demo/demo1](../demo/demo1.cpp)

### 获得电机数据
电机解算后的数据位于`motor.data_`，包含如下信息

| 变量                      | 内容                    |
|-------------------------|-----------------------|
| rotor_angle             | 转子角度 (rad)            |
| rotor_angular_velocity  | 转子角速度 (rad/s)         |
| rotor_linear_velocity   | 转子线速度 (m/s, 角速度 * 半径) |
| output_angular_velocity | 输出角速度 (rad/s)         |
| output_linear_velocity  | 输出线速度 (m/s, 角速度 * 半径) |

电机获得的原始数据位于`motor.motor_measure_`，包含如下信息

| 变量            | 内容         |
|---------------|------------|
| ecd           | 编码器值       |
| speed_rpm     | 转子转速 (rpm) |
| given_current | 电流值        |
| temperate     | 温度         |

### 加入pid控制
#### 创建pid控制器

我们提供了两种预设的pid控制器，他们位于`pid.hpp`，详细的使用可以查阅[tutorials/pid](pid.md)

我们也提供了一些预设的pid参数，位于`config.hpp`，可以快速的启动**M3508**或**M6020**

下面的例子创建了一个增量式的pid控制器
```c++
Pid::PidPosition pid(M3508_SPEED_PID_CONFIG, motor.data_.output_angular_velocity);
```
- **PidPosition**的构造器接受两个参数，一个PidConfig和一个float &，分别表示pid参数和要控制的量
- 这里创建了一个增量式pid用于控制motor的输出角速度

#### 接入电机
提供两种方法方便的接入电机
1. 流式输入数据
    ```c++
    3 >> pid >> motor;
    ```
   这种方式允许你将控制器通过`>>`运算符流式的连接起来，表示 3 先输入 pid 控制器，随后输入电机
2. 和电机绑定
    ```c++
   motor.setCtrl(pid);
   motor.set(3);
    ```
   - 这种方法可以设置motor的控制器列表，将pid与motor绑定，之后`motor.set`函数都会先经过控制器列表的计算后才输入电机
   - 如果你仍想要不经过控制器列表，直接控制执行器，你可以调用`motor.set_directly`方法直接设置电流值

这个demo位于[demo/demo2](../demo/demo2.cpp)

### 多级控制 / 控制系统
为了方便的实现多级控制，我们提供了一种通用的控制器类型`ControllerList`(控制器列表)，它位于头文件`controller.hpp`

`ControllerList`可以接受任何的控制器 / 控制器组合，就像下面的例子
```c++
Pid::PidPosition pid1(...), pid2(...);
ControllerList sys;

sys = pid1;
sys = pid1 >> pid2;
```
控制器之间用`>>`运算符组合即可变成新的控制器，同样的，他们也可以被绑定进电机(执行器)
```c++
motor.setCtrl(pid1 >> pid2);
```
当然你可以继续使用流式控制，如果你喜欢的话
```c++
3 >> pid1 >> pid2 >> motor;
```
我们提供了一个串行pid控制的**M6020**的demo，它位于[demo/demo3](../demo/demo3.cpp)
