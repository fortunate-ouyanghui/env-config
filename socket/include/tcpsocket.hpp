#include <cstdio>
#include <unistd.h>
#include <string>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
using namespace std;

//该值表示同一时间能够接收多少客户端连接
#define MAX_LISTEN 5  
#define CHECK_RET(q) if((q)==false) return -1;

class TcpSocket
{
public:
    TcpSocket():_sockfd(-1){}

    bool Socket()
    {
        /**
         * @brief 创建套接字
         * @param  __domain：地址域：AF_INET->IPv4 AF_INET6->IPv6
         * @param __type：套接字类型：SOCK_STREAM->流式套接字 SOCK_DGRAM->数据报套接字
         * @param __protocol:协议类型：IPPROTO_TCP->TCP IPPROTO_UDP->UDP
         * @return 返回值大于0：成功 返回值小于0：失败
         */
        _sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if(_sockfd<0)
            return false;
        return true;
    }

    bool Bind(const string& ip,uint16_t port)
    {   
        struct sockaddr_in addr;
        addr.sin_family=AF_INET;
        addr.sin_addr.s_addr=inet_addr(ip.c_str());
        addr.sin_port=htons(port);
        socklen_t len=sizeof(struct sockaddr_in);  

        /**
         * @brief 为套接字绑定地址信息
         * @param __fd:待绑定地址信息的套接字句柄
         * @param __addr:要绑定的地址信息结构体
         * @param __len：地址信息结构体的长度
         * @return 返回值大于0：成功 返回值小于0：失败
         */
        int ret=bind(_sockfd,(struct sockaddr *)&addr,len);
        if(ret<0)
        {
            perror("bind error");
            return false;
        }
        return true;
    }

    bool Listen(int backlog=MAX_LISTEN)
    {
        /**
         * @brief 将套接字设置为监听状态
         * @param __fd:该值表示要将哪个套接字设置为监听状态
         * @param __n：该值表示同一时间能够接收多少客户端连接
         * @return 返回值大于0：成功 返回值小于0：失败
         */
        int ret=listen(_sockfd,backlog);
        if(ret<0)
        {
            perror("listen error");
            return false;
        }
       return true;
    }

    bool Accept(TcpSocket* new_sock,string* ip=NULL,uint16_t* port=NULL)
    {
        struct sockaddr_in addr;
        socklen_t len=sizeof(struct sockaddr_in);

         /**
         * @brief 获取新建连接,返回用于双方通信的套接字句柄
         * @param __fd 服务端的套接字
         * @param __addr 得到客户端地址信息结构体
         * @param __addr_len 得到地址信息结构体的长度
         * @return 成功（大于0）： 失败：负数
         */
        int new_sockfd=accept(_sockfd,(struct sockaddr*)&addr,&len);
        if(new_sockfd<0)
        {
            perror("accept error");
            return false;
        }
        else
        {
            new_sock->_sockfd=new_sockfd;
            // 检查 ip 是否为非空指针
            if (ip != nullptr) 
            {
                *ip = inet_ntoa(addr.sin_addr);
            }

            // 检查 port 是否为非空指针
            if (port != nullptr) 
            {
                *port = ntohs(addr.sin_port);
            }
            return true;
        }
    }

    bool Recv(string* buf)
    {
        char tmp[4096]={0};

        /**
         * @brief 数据接收
         * @param __fd 通信套接字句柄
         * @param __buf 接收的数据
         * @param __n 缓冲区的最大容量
         * @param __flags 填0
         * @return 成功返回接收数据的长度 0：表示断开连接 负数：出错
         */
        int ret=recv(_sockfd,tmp,4096,0);
        if(ret<0)
        {
            perror("recv error");
            return false;
        }
        else if(ret==0)
        {
            perror("connect broken");
            return false;
        }
        else
        {
            buf->assign(tmp,ret);
            return true;
        }

    }

    bool Send(const string& data)
    {
        /**
         * @brief 数据发送
         * @param __fd 通信套接字句柄
         * @param __buf 发送的数据
         * @param __n 缓冲区的最大容量
         * @param __flags 填0
         * @return 成功返回发送数据的长度 0：表示断开连接 负数：出错
         */
        int ret=send(_sockfd,data.c_str(),data.size(),0);
        if(ret<0)
        {
            perror("send error");
            return false;
        }
        return true;
    }

    
    bool Close()
    {
        if(_sockfd>0)
        {
            /**
             * @brief 关闭套接字
             */
            close(_sockfd);
            _sockfd=-1;
        }
        return true;
    }

    bool Connect(const string& ip,uint16_t port)
    {
        struct sockaddr_in addr;
        addr.sin_family=AF_INET;
        addr.sin_port=htons(port);
        addr.sin_addr.s_addr=inet_addr(ip.c_str());
        socklen_t len=sizeof(struct sockaddr_in);

        /**
         * @brief 向服务端发起连接请求
         * @param __fd 客户端套接字
         * @param __addr 客户端地址结构体
         * @param __len 客户端地址结构体大小
         * @return 
         */
        int ret = connect(_sockfd, (struct sockaddr*)&addr, len); 
        if(ret<0)
        {
            perror("connect error");
            return false;
        }
        else
            return true;
    }

private:
    int _sockfd;//服务端/客户端套接字句柄
};