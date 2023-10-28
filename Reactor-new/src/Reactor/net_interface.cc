#include "net_interface.h"
int Net_Interface::Sock()
{
    int _fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_fd <= 0)
    {
        printf("socket fails");
        exit(-1);
    }
    return _fd;
}


int Net_Interface::Conncet(std::string sip, uint32_t sport)
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

int Net_Interface::Bind(int _fd, uint32_t port)
{
    sockaddr_in sin = {0};
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = INADDR_ANY;
    return bind(_fd, (sockaddr *)&sin, sizeof(sin));
}

int Net_Interface::Listen(int _fd,uint32_t backlog)
{
    return listen(_fd, backlog);
}

int Net_Interface::Accept(int _fd)
{
    sockaddr_in sin = {0};
    memset(&sin, 0, sizeof(sin));
    socklen_t len = sizeof(sin);
    int conn_fd = accept(_fd, (sockaddr *)&sin, &len);
    return conn_fd;
}

ssize_t Net_Interface::Recv(int fd,std::string& buffer,uint32_t len)
{
    char *temp = new char[len];
    memset(temp, 0, len);
    ssize_t ret = recv(fd, temp, len, 0);
    if(ret <= 0) return ret;
    buffer.assign(temp,ret);
    return ret;
}

ssize_t Net_Interface::Send(int fd,const std::string& buffer,uint32_t len)
{
    return send(fd, buffer.c_str() ,len,0);
}