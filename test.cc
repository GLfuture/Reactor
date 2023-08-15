#include"Reactor.hpp"
#include "Server_Socket/Server_Socket.hpp"
using namespace Reactor_NSP;
#define PORT 9999
#define BACKLOG 10
#define EVENT_NUM 1024
#define BUFER_SIZE 1024
//一定要传引用，因为定时任务会改变reactor的状态，导致段错误
void Accept_cb(Reactor &R , Server_Ptr server)
{
    int clientfd = server->Accept();
    R.Add_Reactor(clientfd,EPOLLIN);
}
void Timeout_cb()
{
    printf("调用\n");
}

void Read_cb(Reactor &R ,Server_Ptr server)
{
    Conn_Ptr client = server->Get_Conn(R.Get_Now_Event().data.fd);
    server->Set_Buffer_Size(2048);
    //std::cout<<R.Get_Now_Event().data.fd<<std::endl;
    if(server->Recv(R.Get_Now_Event().data.fd)==0)
    {
        server->Del_Client(R.Get_Now_Event().data.fd);
        server->Close(R.Get_Now_Event().data.fd);
        return ;
    }
    Timer_Ptr timer = R.Set_Timeout_cb(1,10,Timer::TYPE_ONCE,std::bind(Timeout_cb));
    string buffer = client->Get_Rbuffer();
    std::cout<<buffer<<std::endl;
    client->Set_Wbuffer(buffer);
    server->Send(R.Get_Now_Event().data.fd);
    client->Clean_Rbuffer();
}

int main()
{
    Reactor R(EVENT_NUM,BUFER_SIZE);
    Server_Ptr server=R.Get_Server();
    server->Init_Sock(PORT,BACKLOG);
    R.Add_Reactor(server->Get_Sock(),EPOLLIN);
    //设置非阻塞
    R.Set_No_Block(server->Get_Sock());
    R.Set_Accept_cb(bind(Accept_cb,std::ref(R),server));
    R.Set_Read_cb(bind(Read_cb,std::ref(R),server));
    R.Event_Loop();
    return 0;
}