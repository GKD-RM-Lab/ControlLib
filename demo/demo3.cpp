#include "dji_motor.hpp"
#include "io.hpp"
#include "controller.hpp"
#include "pid.hpp"
#include "config.hpp"
#include "exiter.hpp"

int main() {
    IO::io<CAN>.insert("can0");
    Hardware::DJIMotorManager::start();

    Hardware::DJIMotor motor(6020, "can0", 1);
    motor.enable();

    Pid::PidRad pid1(M6020_POSITION_PID_CONFIG, motor.data_.rotor_angle);
    Pid::PidPosition pid2(M6020_SPEED_PID_CONFIG, motor.data_.rotor_angular_velocity);

    motor.setCtrl(pid1 >> pid2);
    Exiter::Exiter exiter;
    while (!exiter.exit()) {
        motor.set(3);
        UserLib::sleep_ms(1);
    }
}
