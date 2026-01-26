#include "dji_motor.hpp"
#include "io.hpp"
#include "pid.hpp"
#include "config.hpp"
#include "exiter.hpp"

int main() {
    IO::io<CAN>.insert("can0");
    Hardware::DJIMotorManager::start();


    Hardware::DJIMotor motor(3508, "can0", 1);
    motor.enable();

    Pid::PidPosition pid(M3508_SPEED_PID_CONFIG, motor.data_.output_angular_velocity);
    Exiter::Exiter exiter;
    while (!exiter.exit()) {
        3 >> pid >> motor;
        UserLib::sleep_ms(1);
    }
}
