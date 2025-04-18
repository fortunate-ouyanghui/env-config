#include <cstdio>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <iostream>
#include <string.h>
#include <openssl/aes.h>
using namespace std;


void *client_handler(void *arg);//回调函数
int prepare(const char* ip,uint16_t port);//准备阶段
void AES_ECB_encrypt(const unsigned char *input, unsigned char *output, const unsigned char *key);// AES加密
void AES_ECB_decrypt(const unsigned char *input, unsigned char *output, const unsigned char *key);// AES解密
void authenticate(int connfd,unsigned char* decrypted);//认证函数
unsigned char key[16] = "paddddddakjkjkl";// 密钥（AES-128位密钥）
unsigned char decrypted[1024];// 解密后的数据
unsigned char encrypted[1024];//加密后的数据
int flag=1;

int main() 
{
    const char* ip = "192.168.250.242";
    uint16_t port=8888;

    struct sockaddr_in cli_addr;
    socklen_t clilen=sizeof(cli_addr);

    //准备
    int listenfd=prepare(ip,port);

    while (true) 
    {
        //连接
        int connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &clilen);
        
        if (connfd < 0)
        {
            perror("连接失败");
        } 
        else 
        {
            perror("成功连接到客户端");
            
            pthread_t tid;//线程标识符ID
            pthread_create(&tid, NULL, client_handler, (void *)&connfd);
            pthread_detach(tid); // 自动bool authenticate();//认证函数回收线程资源
        }
    }

    close(listenfd);
    return 0;
}


/**
 * @brief 准备阶段：关联ip，端口及开启监听
 * @return 描述符1
 */
int prepare(const char* ip,uint16_t port)
{
    int listened=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    struct sockaddr_in srv_addr;
    srv_addr.sin_family=AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(ip);
    srv_addr.sin_port = htons(port);
    
    bind(listened, (struct sockaddr *)&srv_addr, sizeof(srv_addr));

    listen(listened, 5);

    return listened;
}


/**
 * @brief 处理客户端消息
 * @param arg 描述符2的地址
 */
void *client_handler(void *arg) 
{
    int connfd = *((int *)arg);
    unsigned char cli_msg[1024];
    unsigned char srv_msg[1024];
    ssize_t ret;

    while((ret = read(connfd, cli_msg, sizeof(cli_msg))) > 0) 
    {
        AES_ECB_decrypt(cli_msg,decrypted,key);
        decrypted[1023] = '\0';

        if(strcmp((char*)decrypted,"退出对话！")==0)
        {
            exit(-1);
        }

        cout << "客户端: " << decrypted << endl;
        
        //判断是否认证成功
        if(flag)
        {
            authenticate(connfd,decrypted);
            unsigned char buffer[1024]="验证成功";
            AES_ECB_encrypt(buffer,encrypted,key);
            encrypted[1023]='\0';
            write(connfd, encrypted, sizeof(buffer));
            continue;
        }


        cout<<"请输入：";
        cin>>srv_msg;
        AES_ECB_encrypt(srv_msg,encrypted,key);
        encrypted[1023]='\0';
        write(connfd, encrypted, sizeof(srv_msg));
    }

    close(connfd);
    pthread_exit(NULL);
}



/**
 * @brief AES(ECB)加密
 * @param input 待加密数据
 * @param output 加密后的数据
 * @param key 密钥
 */
void AES_ECB_encrypt(const unsigned char *input, unsigned char *output, const unsigned char *key) 
{
    AES_KEY aes_key;
    /**
     * @brief 设置加密密钥
     * @param key 指向密钥的指针
     * @param aes_key AES_KEY对象指针
     * @return 0成功，-1表示key，aes_key为空，-2表示密钥长度不是128，192，256
     */
    if (AES_set_encrypt_key(key, 128, &aes_key) < 0) 
    {
        fprintf(stderr, "无法设置加密密钥\n");
    }
    
    
    /**
     * @brief AES加密msg
     * @param input 待加密数据的指针
     * @param output 计算后输出数据的指针
     * @param aes_key 密钥
     * @param AES_ENCRYPT代表加密
     */
    for (int i = 0; i <1024 ;i+=16)
    {
        AES_ecb_encrypt(input+i, output+i, &aes_key, AES_ENCRYPT);
    }
}


/**
 * @brief AES(ECB)解密
 * @param input 待解密数据
 * @param output 解密后的数据
 * @param key 密钥
 */
void AES_ECB_decrypt(const unsigned char *input, unsigned char *output, const unsigned char *key)
{
    AES_KEY aes_key;
    /**
     * @brief 设置解密密钥flag_KEY对象指针
     * @return 0成功，-1表示key，aes_key为空，-2表示密钥长度不是128，192，256
     */
    if (AES_set_decrypt_key(key, 128, &aes_key) < 0) 
    {
        fprintf(stderr, "无法设置解密密钥\n");
    }


    /**
     * @brief AES解密单个数据块（16个字节）
     * @param input+i 需要解密的数据
     * @param output+1 计算后输出的数据
     * @param aes_key 密钥
     * @param AES_DECRYPT代表解密
     */
    for(int i=0;i<1024;i+=16)
    {
        AES_ecb_encrypt(input+i, output+i, &aes_key, AES_DECRYPT);
    }
}


/**
 * @brief 认证函数
 * @param connfd 描述符2
 * @param decrypted 客户端发来的认证信息
 */
void authenticate(int connfd,unsigned char* decrypted)
{
    if(strcmp((char*)decrypted,"device001:123456")!=0)
    {
        flag=0;
        unsigned char buffer[1024]="认证失败！";
        AES_ECB_encrypt(buffer,encrypted,key);
        encrypted[1023]='\0';
        write(connfd, encrypted, sizeof(buffer));
        close(connfd);
        exit(-1);
    }
    flag=0;
}