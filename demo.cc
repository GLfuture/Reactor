#include"Reactor.hpp"
void Accept_cb(Reactor R , Server_Ptr server)
{
    int clientfd = server->Accept();
    R.Add_Reactor(clientfd,EPOLLIN);
}

void Read_cb(Reactor R ,Server_Ptr server)
{
    Client_Ptr client = server->Get_Client(R.Get_Now_Event().data.fd);
    server->Set_Buffer_Size(2048);
    //std::cout<<R.Get_Now_Event().data.fd<<std::endl;
    if(server->Recv(R.Get_Now_Event().data.fd)==0)
    {
        server->Del_Client(R.Get_Now_Event().data.fd);
        server->Close(R.Get_Now_Event().data.fd);
        return ;
    }
    string buffer = client->Get_Rbuffer();
    
    std::cout<<buffer<<std::endl;
    client->Set_Wbuffer(buffer);
    server->Send(R.Get_Now_Event().data.fd);
    client->Clean_Rbuffer();
}


int main()
{
    Reactor R;
    Server_Ptr server=R.Get_Server();
    server->Init_Sock(9999,10);
    R.Add_Reactor(server->Get_Sock(),EPOLLIN);
    //设置非阻塞
    R.Set_No_Block(server->Get_Sock());
    R.Accept_cb=bind(Accept_cb,R,server);
    R.Read_cb=bind(Read_cb,R,server);
    R.Event_Loop();
    return 0;
}