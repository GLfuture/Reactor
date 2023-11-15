/*
 * @Description: 
 * @Version: 1.0
 * @Author: Gong
 * @Date: 2023-09-30 11:59:38
 * @LastEditors: Gong
 * @LastEditTime: 2023-10-03 12:59:03
 */

#include "server.h"

int Server_Base::Start_Timer()
{
    return timermanager.Create_Timerfd();
}

Server_Base::Server_Base(int fd, Net_Interface_Base::Ptr &interface)
{
    this->_fd = fd;
    this->interface = interface;
}

int Server_Base::Init_Epoll_Fd()
{
    epfd = epoll_create(1);
    return epfd;
}

int Server_Base::Accept()
{
    return  interface->Accept(_fd);
}


ssize_t Server_Base::Recv(Tcp_Conn_Base::Ptr &conn_ptr, uint32_t len)
{
    std::string buffer="";
    ssize_t ret = interface->Recv(conn_ptr->Get_Conn_fd(),buffer,len);
    if (ret <= 0)
        return ret; 
    conn_ptr->Appand_Rbuffer(buffer);
    return ret;
}

ssize_t Server_Base::Send(const Tcp_Conn_Base::Ptr &conn_ptr, uint32_t len)
{
    return interface->Send(conn_ptr->Get_Conn_fd(),std::string(conn_ptr->Get_Wbuffer()),len);
}

Tcp_Conn_Base::Ptr Server_Base::Connect(std::string sip,uint32_t sport)
{
    int fd = interface->Conncet(sip,sport);
    if(fd <= 0) return nullptr;
    Tcp_Conn_Base::Ptr conn = std::make_shared<Tcp_Conn_Base>(fd);
    return conn;
}

Tcp_Conn_Base::Ptr Server_Base::Get_Conn(int fd)
{
    
    map<uint32_t,Tcp_Conn_Base::Ptr>::iterator it = connections.find(fd);
    if(it == connections.end()) return nullptr;
    return it->second;
}

void Server_Base::Add_Conn(const Tcp_Conn_Base::Ptr &conn_ptr)
{
    std::lock_guard lock(this->mtx);
    this->connections[conn_ptr->Get_Conn_fd()] = conn_ptr;
}


int Server_Base::Close(int fd)
{
    return close(fd);
}

void Server_Base::Clean_Conns()
{
    for (map<uint32_t, Tcp_Conn_Base::Ptr>::iterator it = connections.begin(); it != connections.end(); it++)
    {
        Close((*it).first);
        it = Del_Conn((*it).first);
        if(it != connections.end()){
            it--;
        }
    }
}


map<uint32_t, Tcp_Conn_Base::Ptr>::iterator Server_Base::Del_Conn(int fd)
{
    std::lock_guard lock(this->mtx);
    map<uint32_t,Tcp_Conn_Base::Ptr>::iterator it = connections.find(fd);
    if(it == connections.end()) return it;
    return connections.erase(it);
}
