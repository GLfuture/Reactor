#include"reactor.h"
#include <sys/socket.h>
#define PORT 9999
#define BACKLOG 10
#define EVENT_NUM 1024
//一定要传引用，因为定时任务会改变reactor的状态，导致段错误
void Accept_cb(Reactor &R , Server_Ptr server)
{
    int clientfd = server->Accept();
    Tcp_Conn_Ptr conn = std::make_shared<Tcp_Conn_Base>(clientfd);
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
    Tcp_Conn_Ptr client = server->Get_Conn(clientfd);
    //std::cout<<R.Get_Now_Event().data.fd<<std::endl;
    int rlen=server->Recv(client,1024);
    if(rlen == 0)
    {
        server->Close(clientfd);
        R.Del_Reactor(clientfd,EPOLLIN);
        return ;
    }
    Timer_Ptr timer = R.Set_Timeout_cb(1,10,Timer::TYPE_ONCE,std::bind(Timeout_cb));
    string_view buffer = client->Get_Rbuffer();
    std::cout<<buffer<<std::endl;
    client->Appand_Wbuffer(string(buffer));
    int wlen= server->Send(client,rlen);
    client->Erase_Rbuffer(rlen);
    client->Erase_Wbuffer(wlen);
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
    R.Set_Exit_cb(std::bind(Exit_cb));
    R.Event_Loop(server,-1);
    return 0;
}
