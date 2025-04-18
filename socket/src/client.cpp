#include <iostream>
#include <string>
#include "tcpsocket.hpp"

int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        cout<<"usage:./tcp_client ip port"<<endl;
        return -1;
    }
    string srv_ip=argv[1];
    uint16_t srv_port=stoi(argv[2]);

    TcpSocket sock;
    CHECK_RET(sock.Socket());
    CHECK_RET(sock.Connect(srv_ip,srv_port));
    while(1)
    {
        string buf;
        cout<<"client say:";
        cin>>buf;
        sock.Send(buf);
        buf.clear();
        sock.Recv(&buf);
        cout<<"server say:"<<buf<<endl;

    }
    sock.Close();

    return 0;
}