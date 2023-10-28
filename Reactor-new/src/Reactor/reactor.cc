#include "reactor.h"

Reactor::Reactor(uint16_t event_num)
{
    this->epfd = epoll_create(1);
    this->quit = false;
    this->Accept_cb = NULL;
    this->Read_cb = NULL;
    this->Write_cb = NULL;
    this->event_num = event_num;
    this->event = new epoll_event;
    int timefd = timermanager.Create_Timerfd();
    this->Add_Reactor(timefd, EPOLLIN | EPOLLET);
}


uint16_t Reactor::Add_Reactor(int fd, uint32_t event)
{
    epoll_event ev = {0};
    ev.events = event;
    ev.data.fd = fd;
    uint16_t ret = epoll_ctl(this->epfd, EPOLL_CTL_ADD, fd, &ev);
    return ret;
}

uint16_t Reactor::Del_Reactor(int fd, uint32_t event)
{
    epoll_event ev = {0};
    ev.events = event;
    ev.data.fd = fd;
    uint16_t ret = epoll_ctl(this->epfd, EPOLL_CTL_DEL, fd, &ev);
    return ret;
}

uint16_t Reactor::Mod_Reactor(int fd, uint32_t event)
{
    epoll_event ev = {0};
    ev.events = event;
    ev.data.fd = fd;
    uint16_t ret = epoll_ctl(this->epfd, EPOLL_CTL_MOD, fd, &ev);
    return ret;
}

void Reactor::Set_No_Block(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}

void Reactor::Set_Block(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    flag &= ~O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}


void Reactor::Exit(Server_Ptr& _server)
{
    if (exit_cb != NULL)
        this->exit_cb();
    this->quit = true;
    _server->Clean_Conns();
    delete event;
}

void Reactor::Event_Loop( Server_Ptr& _server , uint16_t timeout)
{
    epoll_event *events = new epoll_event[this->event_num];
    while (!this->quit)
    {
        timermanager.Update_Timerfd();
        int ready = epoll_wait(this->epfd, events, this->event_num, timeout); // 默认死等
        if (ready == 0)
            continue;
        for (int i = 0; i < ready; i++)
        {
            *event = events[i];
            if (events[i].data.fd == _server->Get_Sock())
            {
                if (this->Accept_cb)
                    this->Accept_cb();
            }
            else if (events[i].events == EPOLLIN)
            {
                if (timermanager.Get_Timerfd() == events[i].data.fd)
                    timermanager.Tick();
                else
                {
                    if (this->Read_cb)
                        this->Read_cb();
                }
            }
            else if (events[i].events == EPOLLOUT)
            {
                if (this->Write_cb)
                    this->Write_cb();
            }
        }
    }
}