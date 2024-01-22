#include <iostream>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <time.h>
#include <sys/neutrino.h>

void timerHandler(int sig, siginfo_t *si, void *uc) {
    std::cout << "Timer triggered!" << std::endl;
}

int main() {
    struct sigevent sev;
    std::memset(&sev, 0, sizeof(struct sigevent));
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timerHandler;
    sigaction(SIGRTMIN, &sa, NULL);

    timer_t timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        std::cerr << "Error creating timer" << std::endl;
        exit(EXIT_FAILURE);
    }

    uint64_t createTime = ClockCycles();

    itimerspec its;
    its.it_value.tv_sec = 5;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        std::cerr << "Error starting timer" << std::endl;
        exit(EXIT_FAILURE);
    }

    uint64_t startTime = ClockCycles();
    pause();

    uint64_t cyclesPerSec = SYSPAGE_ENTRY(qtime)->cycles_per_sec;

    double createSec = static_cast<double>(createTime) / cyclesPerSec;
    uint64_t createNsec = static_cast<uint64_t>((createTime % cyclesPerSec) * 1000000000 / cyclesPerSec);

    double startSec = static_cast<double>(startTime) / cyclesPerSec;
    uint64_t startNsec = static_cast<uint64_t>((startTime % cyclesPerSec) * 1000000000 / cyclesPerSec);

    double diffSec = static_cast<double>(startTime - createTime) / cyclesPerSec;
    uint64_t diffNsec = static_cast<uint64_t>(((startTime - createTime) % cyclesPerSec) * 1000000000 / cyclesPerSec);

    std::cout << "Timer creation time: " << createSec << " s " << createNsec << " ns" << std::endl;
    std::cout << "Timer start time: " << startSec << " s " << startNsec << " ns" << std::endl;
    std::cout << "Time difference: " << diffSec << " s " << diffNsec << " ns" << std::endl;

    return 0;
}
