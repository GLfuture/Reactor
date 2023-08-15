#pragma once
#include <cstddef>
#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H
#endif

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <map>
#include <string_view>
#include <string.h>
#include <memory>
#include <unistd.h>
namespace Server_Socket_NSP
{
#define OK 1
#define Conn_Ptr shared_ptr<Conn>

    using std::map;
    using std::shared_ptr;
    using std::string;
    using std::string_view;

    class Server_Socket;
    class Conn
    {
        friend class Server_Socket;

    public:
        Conn()
        {
            this->rbuffer = "";
            this->wbuffer = "";
            this->data = NULL;
        }

        Conn(Conn *client)
        {
            data = client->data;
            rbuffer = client->rbuffer;
            wbuffer = client->wbuffer;
        }

        Conn(const Conn &client)
        {
            data = client.data;
            rbuffer = client.rbuffer;
            wbuffer = client.wbuffer;
        }

        void Set_Wbuffer(string buffer)
        {
            this->wbuffer = buffer;
        }

        string &Get_Rbuffer()
        {
            return this->rbuffer;
        }

        void Clean_Rbuffer()
        {
            this->rbuffer.clear();
        }

    private:
        // uint16_t connfd;//使用reactor模型时不需要此属性，以节约空间
        void *data;
        string rbuffer;
        string wbuffer;
    };

    class Server_Socket
    {
    public:
        Server_Socket(Server_Socket *server)
        {
            this->fd = server->fd;
            this->buffersize = server->buffersize;
            this->connections = server->connections;
            this->buffer = server->buffer;
        }
        Server_Socket(int buffersize)
        {
            this->buffersize = buffersize;
            this->buffer = new char[buffersize];
            memset(this->buffer, 0, buffersize);
        }
        // 初始化sock
        int Init_Sock(uint16_t port, uint16_t backlog)
        {
            fd = socket(AF_INET, SOCK_STREAM, 0);
            if (fd < 0)
            {
                return fd;
            }
            int ret = Bind(port);
            if (ret == -1)
            {
                return ret;
            }
            ret = Listen(backlog);
            if (ret == -1)
            {
                return ret;
            }
            return OK;
        }

        int Accept()
        {
            sockaddr_in sin = {0};
            socklen_t len;
            int connfd = accept(fd, (sockaddr *)&sin, &len);
            if (connfd <= 0)
            {
                return connfd;
            }
            Add_Client(connfd);
            return connfd;
        }

        int Close(uint16_t connfd)
        {
            return close(connfd);
        }

        // 获取client
        Conn_Ptr Get_Conn(uint16_t connfd)
        {
            return this->connections[connfd];
        }

        size_t Send(uint16_t connfd)
        {
            Conn_Ptr cptr = Get_Conn(connfd);
            size_t len = send(connfd, cptr->wbuffer.c_str(), cptr->wbuffer.length(), 0);
            Erase(len, cptr);
            return len;
        }

        size_t Recv(uint16_t connfd)
        {
            Conn_Ptr cptr = Get_Conn(connfd);
            size_t len = recv(connfd, this->buffer, this->buffersize, 0);
            cptr->rbuffer += this->buffer;
            memset(buffer, 0, buffersize);
            return len;
        }

        uint32_t Get_Buffer_Size()
        {
            return this->buffersize;
        }

        // 设置buffer缓冲区大小
        void Set_Buffer_Size(uint32_t size)
        {
            this->buffersize = size;
            delete buffer;
            this->buffer = new char[size];
            memset(buffer, 0, size);
        }
        // 添加client
        void Add_Client(uint16_t connfd)
        {
            Conn *client = new Conn;
            memset(client, 0, sizeof(*client));
            Conn_Ptr client_ptr = std::make_shared<Conn>(client);
            std::pair<uint16_t, Conn_Ptr> mypair(connfd, client_ptr);
            // make_pair报错???
            connections.insert(mypair);
        }

        // 删除client
        void Del_Client(uint16_t connfd)
        {
            this->connections[connfd].reset();
            this->connections.erase(connfd);
        }

        uint16_t Get_Sock()
        {
            return this->fd;
        }

    private:
        void Erase(int len, Conn_Ptr cptr)
        {
            string::iterator it = cptr->wbuffer.begin();
            for (int i = 0; i < len; i++)
            {
                it++;
            }
            cptr->wbuffer.erase(cptr->wbuffer.begin(), it);
        }

        uint16_t Bind(uint16_t port)
        {
            sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            uint16_t ret = bind(fd, (sockaddr *)&addr, sizeof(addr));
            return ret;
        }

        uint16_t Listen(uint16_t backlog)
        {
            uint16_t ret = listen(fd, backlog);
            return ret;
        }

    private:
        uint16_t fd;
        // 每一个用户对应一个fd类型的唯一标识
        map<uint16_t, shared_ptr<Conn>> connections;
        char *buffer; // 读缓冲区
        uint32_t buffersize;
    };
}