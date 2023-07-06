#pragma once
#include"rbtree-timer.hpp"
#include"minheap-timer.hpp"

class TimerManager:public RBtree_TimerManager
{
public:
    int Get_Timerfd()
    {
        return this->timerfd;
    }

    int Create_Timerfd(){
        timerfd = timerfd_create(CLOCK_MONOTONIC,0);
        return timerfd;
    }

    void Update_Timerfd() {
        struct timespec abstime;
        auto iter = timers.begin();
        if (iter != timers.end()) {
            abstime.tv_sec = iter->Get_Expire_Time() / 1000;
            abstime.tv_nsec = (iter->Get_Expire_Time() % 1000) * 1000000;
        } else {
            abstime.tv_sec = 0;
            abstime.tv_nsec = 0;
        }
        struct itimerspec its = {
            .it_interval = {},
            .it_value = abstime
        };
        timerfd_settime(this->timerfd, TFD_TIMER_ABSTIME, &its, nullptr);
    }
private:
    int timerfd;
};