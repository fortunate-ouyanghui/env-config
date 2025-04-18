#include "tcpsocket.hpp"
#include <cstdio>
#include <iostream>

using namespace std;

int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        cout<<"usage:./tcp_srv ip port"<<endl;
        return -1;
    }

    string ip=argv[1];
    uint16_t port=stoi(argv[2]);

    TcpSocket sock;
    //穿件套接字
    CHECK_RET(sock.Socket());
    //为套接字绑定地址信息
    CHECK_RET(sock.Bind(ip,port));
    //开始监听
    CHECK_RET(sock.Listen(5));
    while(1)
    {
        TcpSocket new_sock;//通信套接字对象
        //获取新建连接
        bool ret=sock.Accept(&new_sock,&ip,&port);
        if(ret==false)
        {
            continue;
        }
        string buf;
        new_sock.Recv(&buf);
        cout<<"client say:"<<buf<<endl;
        buf.clear();
        cout<<"server say:";
        cin>>buf;
        new_sock.Send(buf);
    }
    sock.Close();
    return 0;
}