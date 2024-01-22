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

    uint64_t diffTime = startTime - createTime;
    uint64_t nsPerSec = SYSPAGE_ENTRY(qtime)->cycles_per_sec;
    uint64_t sec = diffTime / nsPerSec;
    uint64_t nsec = (diffTime % nsPerSec) * 1000000000 / nsPerSec;

    std::cout << "Timer creation time: " << createTime << " cycles" << std::endl;
    std::cout << "Timer start time: " << startTime << " cycles" << std::endl;
    std::cout << "Time difference: " << sec << " s " << nsec << " ns" << std::endl;

    return 0;
}
