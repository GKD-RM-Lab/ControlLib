#include "pid.hpp"

constexpr Pid::PidConfig M3508_SPEED_PID_CONFIG{
    200.f,
    5.f,
    0.2f,
    30000.0f,
    15000.0f,
};

constexpr Pid::PidConfig M6020_SPEED_PID_CONFIG{
    500.f,
    100.f,
    0.2f,
    30000.0f,
    15000.0f,
};

constexpr Pid::PidConfig M6020_POSITION_PID_CONFIG{
    7.f,
    0.01f,
    0.0f,
    30.0f,
    0.0f,
};