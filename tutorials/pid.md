## Pid::PidConfig

```cpp
struct PidConfig {
        float kp;       // 比例系数
        float ki;       // 积分系数
        float kd;       // 微分系数

        float max_out;  // 最大输出
        float max_iout; // 最大积分输出
};
```

`PidConfig`为基础的Pid参数，未提供任何其他功能。

----
## Pid::PidPosition
```cpp
class PidPosition final : public PidConfig, public Controller;
```
`PidPosition`是位置式Pid控制器，集成了`PidConfig`和`Controller`。

### 成员函数

| 函数名                                                 | 功能                                         |
| ------------------------------------------------------ | -------------------------------------------- |
| PidPosition(const PidConfig &config, const float &ref) | 构造器，传入`PidConfig`和被控制变量的引用    |
| void set(float set_v) override                         | 传入设置值，执行一次计算，重载自`Controller` |
| void clean()                                           | 清空所有状态量，包括积分量和历史误差         |

## Pid::PidRad

```cpp
class PidRad final : public PidConfig, public Controller;
```

`PidRad`是用于角度上的位置式Pid控制器，采用弧度制，继承了`PidConfig`和`Controller`。

### 成员函数

| 函数名                                            | 功能                                         |
| ------------------------------------------------- | -------------------------------------------- |
| PidRad(const PidConfig &config, const float &ref) | 构造器，传入`PidConfig`和被控制变量的引用    |
| void set(float set_v) override                    | 传入设置值，执行一次计算，重载自`Controller` |
| void clean()                                      | 清空所有状态量，包括积分量和历史误差         |

