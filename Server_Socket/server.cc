#include"Server_Socket.hpp"
using namespace std;
int main()
{
    Server_Socket server;
    server.Init_Sock(8080,10);
    int clientfd = server.Accept();
    Client_Ptr client = server.Get_Client(clientfd);
    server.Recv(clientfd,client);
    cout<<client->Get_Rbuffer();
    //清空read_buffer
    client->Clean_Rbuffer();
    client->Set_Wbuffer("hello client");
    server.Send(clientfd,client);
    return 0;
}