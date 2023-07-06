#pragma once
#ifndef TIMER_HPP
#define TIMER_HPP
#endif
#include <iostream>
#include <functional>
#include <set>
#include <sys/timerfd.h>
#include <chrono>
#include <time.h>
class Timer
{
public:
    enum TimerType
    {
        TYPE_BEG = 0,
        TYPE_ONCE = TYPE_BEG,
        TYPE_CIRCLE,
        TYPE_END
    };

    static inline time_t Get_Current_Time()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }

    Timer(uint16_t timer_id, uint64_t interval_time, TimerType timer_type, std::function<void()> &timer_callback)
    {
        this->timerid = timer_id;
        this->type = timer_type;
        this->callback = timer_callback;
        this->Set_Interval_Time(interval_time);
    }

    Timer(const Timer &timer)
    {
        this->timerid = timer.timerid;
        this->time_expire = timer.time_expire;
        this->time_interval = timer.time_interval;
        this->type = timer.type;
        this->callback = timer.callback;
    }

    uint16_t Get_Timerid() const
    {
        return this->timerid;
    }

    uint64_t Get_Expire_Time() const
    {
        return time_expire;
    }

    uint64_t Get_Interval_Time() const
    {
        return time_interval;
    }

    TimerType Get_Timertype() const
    {
        return this->type;
    }

    void Set_Interval_Time(uint64_t time)
    {
        time_interval = time;
        Set_Expire_Time(time_interval);
    }

    bool operator<(const Timer &other) const
    {
        return time_expire < other.time_expire;
    }

private:
    void Set_Expire_Time(uint64_t time)
    {
        time_expire = Get_Current_Time() + time;
    }

private:
    friend class TimerManager;
    friend class RBtree_TimerManager;
    uint16_t timerid; // 定时器id
    TimerType type;
    uint64_t time_interval; // 间隔时间
    uint64_t time_expire;   // 过期时间
    std::function<void()> callback;
};