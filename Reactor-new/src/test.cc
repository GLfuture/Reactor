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
#include <unistd.h>
#define PORT 9999
#define BACKLOG 20
#define EVENT_NUM 1024
#define SERVER_NUM 4

//一定要传引用，因为定时任务会改变reactor的状态，导致段错误
static std::mutex mtx;

void Accept_cb(Reactor::Ptr R , Server_Base::Ptr server)
{
    std::unique_lock lock(mtx);
    int clientfd = server->Accept();
    if(clientfd <= 0) return;
    Tcp_Conn_Base::Ptr conn = std::make_shared<Tcp_Conn>(clientfd);
    server->Add_Conn(conn);
    R->Add_Reactor(server->Get_Epoll_Fd(), clientfd,EPOLLIN);
}
void Timeout_cb()
{
    printf("调用\n");
}

void Exit_cb()
{
    printf("exit\n");
}

void Read_cb(Reactor::Ptr R ,Server_Base::Ptr server)
{
    int clientfd = R->Get_Now_Event().data.fd;
    Tcp_Conn_Base::Ptr tmp_ptr = server->Get_Conn(clientfd);
    if(tmp_ptr ==nullptr) return;
    Tcp_Conn_Base::Ptr client = std::dynamic_pointer_cast<Tcp_Conn>(tmp_ptr);
    //std::cout<<R->Get_Now_Event().data.fd<<std::endl;
    int rlen=server->Recv(client,1024);
    if(rlen == 0)
    {
        server->Close(clientfd);
        server->Del_Conn(clientfd);
        R->Del_Reactor(server->Get_Epoll_Fd(),clientfd,EPOLLIN);
        std::cout<< "close fd: "<<clientfd<<std::endl;
        std::cout<< "connections: " <<server->Get_Conn_Num()<<std::endl;
        return ;
    }
    string_view buffer = client->Get_Rbuffer();
    std::cout<< buffer <<std::endl;
    client->Appand_Wbuffer(string(buffer));
    client->Erase_Rbuffer(rlen);
    Timer::Ptr timer = server->Set_Timeout_cb(1,10,Timer::TYPE_ONCE,std::bind(Timeout_cb));
    R->Mod_Reactor(server->Get_Epoll_Fd(),clientfd,EPOLLOUT);
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void Write_cb(Reactor::Ptr R ,Server_Base::Ptr server)
{
    int clientfd  = R->Get_Now_Event().data.fd;
    Tcp_Conn_Base::Ptr conn = std::dynamic_pointer_cast<Tcp_Conn>(server->Get_Conn(clientfd));
    int len = server->Send(conn , conn->Get_Wbuffer_Length());
    conn->Erase_Wbuffer(len);
    R->Mod_Reactor(server->Get_Epoll_Fd(), clientfd,EPOLLIN);
}

int main()
{
    Reactor::Ptr R[SERVER_NUM];
    Server_Base::Ptr servers[SERVER_NUM];
    Net_Interface_Base::Ptr interface = std::make_shared<Net_Interface>();
    int sock = interface->Sock();
    if(sock <= 0 ){
        perror("socket");
    }
    int ret = interface->Bind(sock,PORT);
    if(ret == -1) {
        perror("bind");
    }
    ret = interface->Listen(sock,BACKLOG);
    if(ret == -1) {
        perror("listen");
    }

    std::thread ths[SERVER_NUM];
    for(int i=0;i<SERVER_NUM;i++)
    {
        if(i==0) R[i]->Set_No_Block(sock);
        R[i] = std::make_shared<Reactor>(EVENT_NUM);
        servers[i] = std::make_shared<Server>(sock,interface);
        servers[i]->callback.Accept_cb = std::bind(Accept_cb,R[i],servers[i]);
        servers[i]->callback.Read_cb = std::bind(Read_cb,R[i],servers[i]);
        servers[i]->callback.Write_cb = std::bind(Write_cb,R[i],servers[i]);
        servers[i]->callback.Exit_cb = std::bind(Exit_cb);
        servers[i]->Init_Epoll_Fd();
        R[i]->Add_Reactor(servers[i]->Get_Epoll_Fd(),sock,EPOLLIN);
        int timerfd = servers[i]->Start_Timer();
        R[i]->Add_Reactor(servers[i]->Get_Epoll_Fd(), timerfd, EPOLLIN | EPOLLET);
        ths[i] = std::thread(&Reactor::Event_Loop,R[i],servers[i],servers[i]->Get_Epoll_Fd(),-1);

    }
    
    for(int i = 0; i < SERVER_NUM ;i++)
    {
        ths[i].join();
    }
    return 0;
}
