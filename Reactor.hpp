#pragma once
#include "Server_Socket/Server_Socket.hpp"
#include <sys/epoll.h>
#include <fcntl.h>
#include<functional>
using std::function;
using Server_Ptr = shared_ptr<Server_Socket>;

//注意一个端口对应一个reactor管理
class Reactor
{
public:
    Reactor(uint16_t port, uint16_t backlog)
    {
        this->epfd = epoll_create(1);
        this->server->Init_Sock(port, backlog);
        this->quit=false;
        this->Accept_cb=NULL;
        this->Read_cb=NULL;
        this->Write_cb=NULL;
    }
    // 返回server对象
    Server_Ptr Get_Server()
    {
        return this->server;
    }

    uint16_t Add_Reactor(int sock, uint32_t event)
    {
        epoll_event ev={0};
        ev.events=event;
        ev.data.fd=sock;
        epoll_ctl(this->epfd, EPOLL_CTL_ADD, sock, &ev);
    }

    uint16_t Del_Reactor(int sock, uint32_t event)
    {
        epoll_event ev={0};
        ev.events=event;
        ev.data.fd=sock;
        epoll_ctl(this->epfd, EPOLL_CTL_DEL, sock, &ev);
    }

    uint16_t Mod_Reactor(int sock, uint32_t event)
    {
        epoll_event ev={0};
        ev.events=event;
        ev.data.fd=sock;
        epoll_ctl(this->epfd, EPOLL_CTL_MOD, sock, &ev);
    }

    // 设置非阻塞
    void Set_No_Block(int fd)
    {
        int flag = fcntl(fd, F_GETFL, 0);
        flag |= O_NONBLOCK;
        fcntl(fd, F_SETFL, flag);
    }
    //设置阻塞
    void Set_Block(int fd)
    {
        int flag = fcntl(fd, F_GETFL, 0);
        flag &= ~O_NONBLOCK;
        fcntl(fd, F_SETFL, flag);
    }

    //设置事件数量
    void Set_Event_Num(uint32_t num)
    {
        this->event_num=num;
    }

    //获取事件数量
    uint32_t Get_Event_Num()
    {
        return this->event_num;
    }

    //事件主循环,默认死等
    void Deal_Event(uint16_t timeout=-1)
    {
        epoll_event events[this->event_num];
        while(!this->quit)
        {
            int ready = epoll_wait(this->epfd,events,this->event_num,timeout);//默认死等
            for(int i=0;i<ready;i++)
            {
                if(events[i].data.fd==server->Get_Sock()){
                    if(!this->Accept_cb) this->Accept_cb();
                }else if(events[i].events==EPOLLIN){
                    if(!this->Read_cb) this->Read_cb();
                }else if(events[i].events==EPOLLOUT){
                    if(!this->Write_cb) this->Write_cb();
                }
            }

        }
    }

public:
    function<void()> Accept_cb;
    function<void()> Read_cb;
    function<void()> Write_cb;
private:
    uint16_t epfd;
    Server_Ptr server;
    bool quit;
    uint32_t event_num;
};