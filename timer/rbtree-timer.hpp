#pragma once
#ifndef RBTREE_TIMER_HPP
#define RBTREE_TIMER_HPP
#endif
#include "timer.hpp"
using std::multiset;

class RBtree_TimerManager
{
public:
    void Add_Timer(uint16_t timer_id, uint64_t interval_time, Timer::TimerType timer_type, std::function<void()> &timer_callback)
    {
        Timer timer(timer_id, interval_time, timer_type, timer_callback);
        timers.insert(timer);
    }

    void Del_Timer(uint16_t timerid)
    {
        for (multiset<Timer>::iterator it = timers.begin(); it != timers.end(); it++)
        {
            if (it->Get_Timerid() == timerid)
            {
                timers.erase(it);
                break;
            }
        }
    }

    void Start()
    {
        while (Is_Ready())
        {
            Timer timer = (*timers.begin());
            timers.erase(timers.begin());
            timer.callback();
            if (timer.Get_Timertype() == Timer::TYPE_CIRCLE)
            {
                Add_Timer(timer.Get_Timerid(), timer.Get_Interval_Time(), timer.Get_Timertype(), timer.callback);
            }
        }
    }
    
    // 判断最近的定时器是否到期
    bool Is_Ready()
    {
        time_t now = Timer::Get_Current_Time();
        if (!timers.empty() && timers.begin()->Get_Expire_Time() <= now)
            return true;
        return false;
    }

protected:
    multiset<Timer> timers;
};