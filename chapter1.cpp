#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <signal.h>
#include <time.h>

void timerHandler(int sig, siginfo_t *si, void *uc) {
    std::cout << "Таймер сработал!" << std::endl;
}

int main() {
    struct sigevent sev;
    std::memset(&sev, 0, sizeof(struct sigevent));
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;

    timer_t timerid;
    if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
        perror("Ошибка при создании таймера");
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
        perror("Ошибка при запуске таймера");
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

    std::cout << "Время создания таймера: " << createTime.tv_sec << " с " << createTime.tv_nsec << " нс" << std::endl;
    std::cout << "Время запуска таймера: " << startTime.tv_sec << " с " << startTime.tv_nsec << " нс" << std::endl;
    std::cout << "Разница времени: " << diffTime.tv_sec << " с " << diffTime.tv_nsec << " нс" << std::endl;

    return 0;
}
