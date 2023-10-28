/*
 * @Description: 
 * @Version: 4.9
 * @Author: Gong
 * @Date: 2023-09-30 11:59:38
 * @LastEditors: Gong
 * @LastEditTime: 2023-10-05 07:19:42
 */
#include"reactor.h"
#include <sys/socket.h>
#include <thread>
#define PORT 9998
#define BACKLOG 10
#define EVENT_NUM 1024
//一定要传引用，因为定时任务会改变reactor的状态，导致段错误
using Server_Ptr = std::shared_ptr<Server>;
using Tcp_Conn_Ptr = std::shared_ptr<Tcp_Conn>;

void Accept_cb(Reactor &R , Server_Ptr server)
{
    int clientfd = server->Accept();
    Tcp_Conn_Ptr conn = std::make_shared<Tcp_Conn>(clientfd);
    server->Add_Conn(conn);
    R.Add_Reactor(clientfd,EPOLLIN);
}
void Timeout_cb()
{
    printf("调用\n");
}

void Exit_cb()
{
    printf("exit\n");
}

void Read_cb(Reactor &R ,Server_Ptr server)
{
    int clientfd = R.Get_Now_Event().data.fd;
    Tcp_Conn_Ptr client = std::dynamic_pointer_cast<Tcp_Conn>(server->Get_Conn(clientfd));
    //std::cout<<R.Get_Now_Event().data.fd<<std::endl;
    int rlen=server->Recv(client,1024);
    if(rlen == 0)
    {
        server->Close(clientfd);
        server->Del_Conn(clientfd);
        R.Del_Reactor(clientfd,EPOLLIN);
        std::cout<< "close fd: "<<clientfd<<std::endl;
        std::cout<< "connections: " <<server->Get_Conn_Num()<<std::endl;
        return ;
    }
    Reactor::Timer_Ptr timer = R.Set_Timeout_cb(1,10,Timer::TYPE_ONCE,std::bind(Timeout_cb));
    string_view buffer = client->Get_Rbuffer();
    std::cout<<buffer<<std::endl;
    client->Appand_Wbuffer(string(buffer));
    client->Erase_Rbuffer(rlen);
    R.Mod_Reactor(clientfd,EPOLLOUT);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void Write_cb(Reactor &R ,Server_Ptr server)
{
    int clientfd  = R.Get_Now_Event().data.fd;
    Tcp_Conn_Ptr conn = std::dynamic_pointer_cast<Tcp_Conn>(server->Get_Conn(clientfd));
    int len = server->Send(conn , conn->Get_Wbuffer_Length());
    conn->Erase_Wbuffer(len);
    R.Mod_Reactor(clientfd,EPOLLIN);
}

int main()
{
    Reactor R(EVENT_NUM);
    Server_Ptr server = std::make_shared<Server_Base>();
    server->Bind(PORT);
    server->Listen(BACKLOG);
    R.Add_Reactor(server->Get_Sock(),EPOLLIN);
    //设置非阻塞
    R.Set_No_Block(server->Get_Sock());
    R.Set_Accept_cb(std::bind(Accept_cb,std::ref(R),server));
    R.Set_Read_cb(std::bind(Read_cb,std::ref(R),server));
    R.Set_Write_cb(std::bind(Write_cb,std::ref(R),server));
    R.Set_Exit_cb(std::bind(Exit_cb));
    R.Event_Loop(server,-1);
    return 0;
}
