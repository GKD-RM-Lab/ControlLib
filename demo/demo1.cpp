#include "dji_motor.hpp"
#include "io.hpp"
#include "exiter.hpp"
int main() {
    IO::io<CAN>.insert("can0");
    Hardware::DJIMotorManager::start();

    Hardware::DJIMotor motor(3508, "can0", 1);
    motor.enable();

    Exiter::Exiter exiter;
    while (!exiter.exit()) {
        motor.set(5000);
        UserLib::sleep_ms(1);
    }
}
