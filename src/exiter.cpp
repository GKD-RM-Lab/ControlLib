#include "exiter.hpp"

#include <csignal>
#include <stdexcept>

static volatile sig_atomic_t exit_flag = 0;
static bool exiter_inited = false;

static void handle_sigint(int) {
    exit_flag = 1;
}

namespace Exiter {

Exiter::Exiter() {
    if (exiter_inited)
        throw std::runtime_error("Multiple Exiter instances!");
    std::signal(SIGINT, handle_sigint);
    exiter_inited = true;
}

bool Exiter::exit() const {
    return exit_flag != 0;
}

}  // namespace Exiter
