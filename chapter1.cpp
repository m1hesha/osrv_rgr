#include <iostream>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <time.h>

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

    timespec createTime;
    clock_gettime(CLOCK_REALTIME, &createTime);

    itimerspec its;
    its.it_value.tv_sec = 5;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;

    if (timer_settime(timerid, 0, &its, NULL) == -1) {
        std::cerr << "Error starting timer" << std::endl;
        exit(EXIT_FAILURE);
    }

    timespec startTime;
    clock_gettime(CLOCK_REALTIME, &startTime);
    pause();

    timespec diffTime;
    diffTime.tv_sec = startTime.tv_sec - createTime.tv_sec;
    diffTime.tv_nsec = startTime.tv_nsec - createTime.tv_nsec;
    if (diffTime.tv_nsec < 0) {
        diffTime.tv_sec--;
        diffTime.tv_nsec += 1000000000;
    }

    std::cout << "Timer creation time: " << createTime.tv_sec << " s " << createTime.tv_nsec << " ns" << std::endl;
    std::cout << "Timer start time: " << startTime.tv_sec << " s " << startTime.tv_nsec << " ns" << std::endl;
    std::cout << "Time difference: " << diffTime.tv_sec << " s " << diffTime.tv_nsec << " ns" << std::endl;

    return 0;
}
