#pragma once
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <map>
#include <memory>
#include "conn.h"
using std::map;
using std::shared_ptr;
enum Error_Code
{
    OK = 0,
    SOCKET_ERR = -1,
    CONNECT_ERR = -2,
};
const char* arr[]=
{
    "successful",
    "socket function is failed",
    "connect function is failed",
};

using Tcp_Conn_Ptr = shared_ptr<Tcp_Conn_Base>;
class Server_Base
{
public:
    Server_Base();

    Tcp_Conn_Ptr Conncet(string sip,uint32_t sport);

    int Bind(uint32_t port);

    int Listen(uint32_t backlog);

    int Accept();

    ssize_t Recv(const Tcp_Conn_Ptr& conn_ptr,uint32_t len);

    ssize_t Send(const Tcp_Conn_Ptr& conn_ptr,uint32_t len);

    Tcp_Conn_Ptr Get_Conn(int fd) { return connections[fd]; }

    map<uint32_t, Tcp_Conn_Ptr>::iterator Close(int fd);

    void Clean_Conns();

    int Get_Sock() { return _fd; }

private:
    map<uint32_t, Tcp_Conn_Ptr>::iterator Del_Conn(int fd);
    
private:
    int _fd;
    map<uint32_t,Tcp_Conn_Ptr> connections;
};
