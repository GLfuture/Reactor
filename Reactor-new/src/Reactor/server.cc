/*
 * @Description: 
 * @Version: 1.0
 * @Author: Gong
 * @Date: 2023-09-30 11:59:38
 * @LastEditors: Gong
 * @LastEditTime: 2023-10-03 07:23:22
 */

#include "server.h"

Server_Base::Server_Base()
{
    _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd <= 0)
    {
        printf("socket fails");
        exit(-1);
    }
}

int Server_Base::Conncet(string sip, uint32_t sport)
{
    int conn_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (conn_fd <= 0)
        throw SOCKET_ERR;
    sockaddr_in sin = {0};
    memset(&sin, 0, sizeof(sin));
    sin.sin_addr.s_addr = inet_addr(sip.c_str());
    sin.sin_family = AF_INET;
    sin.sin_port = htons(sport);
    int ret = connect(conn_fd, (sockaddr *)&sin, sizeof(sin));
    if (ret == -1)
        throw CONNECT_ERR;
    return conn_fd;
}

int Server_Base::Bind(uint32_t port)
{
    sockaddr_in sin = {0};
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;
    return bind(_fd, (sockaddr *)&sin, sizeof(sin));
}

int Server_Base::Listen(uint32_t backlog)
{
    return listen(_fd, backlog);
}

int Server_Base::Accept()
{
    sockaddr_in sin = {0};
    memset(&sin, 0, sizeof(sin));
    socklen_t len = sizeof(sin);
    int conn_fd = accept(_fd, (sockaddr *)&sin, &len);
    return conn_fd;
}

ssize_t Server_Base::Recv(const Server_Base::Tcp_Conn_Base_Ptr &conn_ptr, uint32_t len)
{
    char *buffer = new char[len];
    memset(buffer, 0, len);
    ssize_t ret = recv(conn_ptr->Get_Conn_fd(), buffer, len, 0);
    if (ret <= 0)
        return ret;
    conn_ptr->Appand_Rbuffer(buffer);
    return ret;
}

ssize_t Server_Base::Send(const Server_Base::Tcp_Conn_Base_Ptr &conn_ptr, uint32_t len)
{
    return send(conn_ptr->Get_Conn_fd(), conn_ptr->Get_Wbuffer().cbegin(), len, 0);
}

void Server_Base::Add_Conn(const Server_Base::Tcp_Conn_Base_Ptr &conn_ptr)
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
    for (map<uint32_t, Server_Base::Tcp_Conn_Base_Ptr>::iterator it = connections.begin(); it != connections.end(); it++)
    {
        Close((*it).first);
        it = Del_Conn((*it).first);
        if(it != connections.end()){
            it--;
        }
    }
}


map<uint32_t, Server_Base::Tcp_Conn_Base_Ptr>::iterator Server_Base::Del_Conn(int fd)
{
    std::lock_guard lock(this->mtx);
    map<uint32_t,Server_Base::Tcp_Conn_Base_Ptr>::iterator it = connections.find(fd);
    if(it == connections.end()) return it;
    return connections.erase(it);
}
