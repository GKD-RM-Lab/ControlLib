## Conroller

```cpp
class Controller;
```

控制器基类，用来封装各种控制器（作为父类）。

### 内容

| 名称                            | 功能                                      |
| ------------------------------- | ----------------------------------------- |
| float out = 0                   | 控制器输出值，子类需要将输出值存入`out`。 |
| virtual void set(float x) = 0   | 设置控制量的虚函数，子类需要重载。        |
| virtual ~Controller() = default | 虚析构。                                  |

### concept

```cpp
template<typename T>
concept ControllerBase = std::is_base_of_v<Controller, std::decay_t<T>>;
```

`Controller`的子类（或本身）



## ControllerList

通用控制器容器，用于存储任何类型的**控制器或控制器列表**。

### 成员函数

| 函数名                                                       | 功能                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| ControllerList() = default                                   | 空构造器。                                                   |
| ControllerList(const ControllerList &ctrl)                   | 拷贝构造器，会进行深拷贝。                                   |
| ControllerList& operator=(const ControllerList &ctrl)        | 拷贝赋值，同拷贝构造。                                       |
| ControllerList(ControllerList &&ctrl) noexcept               | 移动构造器。                                                 |
| ControllerList(const T &ctrl)                                | 构造器，从其他非`ControllerList`类型的控制器进行构造，会对传入的控制器执行拷贝并存储。 |
| ControllerList(const ControllerList &left, const ControllerList &right) | 构造器，将传入的两个控制器按顺序组合为新的构造期，会对传入的控制器执行拷贝并存储。 |
| ControllerList(ControllerList &&left, ControllerList &&right) | 构造器，将传入的两个控制器按顺序组合为新的构造期，会对传入的控制器执行移动并存储。 |
| void set(float v) override                                   | 设置控制值，会按顺序调用所有存储的控制器，并将上一个控制器的`out`值传入下一个控制器的`set`函数。空列表的情况下直接对`out`赋值。 |

### 非成员函数

| 流式输入，调用`set`函数并返回结果。       |
| ----------------------------------------- |
| float operator>>(float v, Controller& c)  |
| float operator>>(float v, Controller&& c) |

| 流式组合，组合两个控制器                                     |
| ------------------------------------------------------------ |
| ControllerList operator>>(const ControllerList &c1, const ControllerList &c2) |
| ControllerList operator>>(ControllerList &&c1, ControllerList &&c2) |
| ControllerList operator>>(ControllerList &&c1, const ControllerList &c2) |
| ControllerList operator>>(const ControllerList &c1, ControllerList &&c2) |

