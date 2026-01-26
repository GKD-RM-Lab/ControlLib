#include "dji_motor.hpp"
#include "io.hpp"
#include "controller.hpp"
#include "pid.hpp"
#include "config.hpp"
#include "serial_interface.hpp"
#include "exiter.hpp"

fp32 yaw = 0;
fp32 pitch = 0;
fp32 roll = 0;
fp32 yaw_rate = 0;
fp32 pitch_rate = 0;
fp32 roll_rate = 0;

void unpack(const Types::ReceivePacket &pkg) {
    yaw = UserLib::rad_format(pkg.yaw * (M_PIf / 180));
    pitch = -UserLib::rad_format(pkg.pitch * (M_PIf / 180));
    roll = UserLib::rad_format(pkg.roll * (M_PIf / 180));
    yaw_rate = -pkg.yaw_v * (M_PIf / 180) / 1000;
    pitch_rate = pkg.pitch_v * (M_PIf / 180) / 1000;
    roll_rate = pkg.roll_v * (M_PIf / 180) / 1000;
}

class Inv final : public Controller {
public:
    void set(const float x) override {
        out = -x;
    }
    ~Inv() override = default;
};


int main() {
    IO::io<CAN>.insert("can0");
    IO::io<SERIAL>.insert("/dev/ttyACM0", 115200, 2000);

    IO::io<SERIAL>["/dev/ttyACM0"]->register_callback([&](const Types::ReceivePacket &rp) {
        unpack(rp);
    });
    Inv inv;

    Hardware::DJIMotorManager::start();

    Hardware::DJIMotor yaw_motor(6020, "can0", 2);
    yaw_motor.enable();
    Hardware::DJIMotor pitch_motor(6020, "can0", 1);
    pitch_motor.enable();

    Pid::PidRad yaw_pid1(M6020_POSITION_PID_CONFIG, yaw);
    Pid::PidPosition yaw_pid2(M6020_SPEED_PID_CONFIG, yaw_rate);

    Pid::PidRad pitch_pid1(M6020_POSITION_PID_CONFIG, pitch);
    Pid::PidPosition pitch_pid2(M6020_SPEED_PID_CONFIG, pitch_rate);

    yaw_motor.setCtrl(yaw_pid1 >> yaw_pid2 >> inv);
    pitch_motor.setCtrl(pitch_pid1 >> pitch_pid2);
    Exiter::Exiter exiter;
    while (!exiter.exit()) {
        yaw_motor.set(0);
        pitch_motor.set(0);
        UserLib::sleep_ms(1);
    }
}
