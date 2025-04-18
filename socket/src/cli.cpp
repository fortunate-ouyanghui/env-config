#include <cstdio>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <iostream>
#include <openssl/aes.h>
using namespace std;

void *recv_handler(void *arg);//回调函数
int Connect(const char* ip,uint16_t port);//请求连接
void AES_ECB_encrypt(const unsigned char *input, unsigned char *output, const unsigned char *key);// AES加密
void AES_ECB_decrypt(const unsigned char *input, unsigned char *output, const unsigned char *key);// AES解密


unsigned char key[16] = "paddddddakjkjkl";// 密钥（AES-128位密钥）
unsigned char encrypted[1024];//加密后的数据
unsigned char decrypted[1024];//解密后的数据


int main() 
{
    const char *ip = "192.168.250.242";
    uint16_t port=8888;
    
    int clifd=Connect(ip,port);

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, recv_handler, (void *)&clifd);

    
    unsigned char msg[1024];
    cout<<"注意：\n"<<"1.如果你输入break表示退出对话！！！\n"<<"2.认证信息由设备名:密码构成。如:device001:123456\n"<<endl;
    cout<<"请输入认证信息:";
    while (true) 
    {
        cin>>msg;
        //AES_ECB_encrypt(msg,encrypted,key);
        encrypted[1023]='\0';

        //如果退出对话，服务端也要退出
        if(strcmp((char*)msg,"break")==0)
        {
            unsigned char buffer[1024]="退出对话！";
            //AES_ECB_encrypt(buffer,encrypted,key);
            encrypted[1023]='\0';
            write(clifd, encrypted, sizeof(buffer));
            exit(-1);
        }

        write(clifd, encrypted,sizeof(msg));
    }

    pthread_join(recv_thread, NULL);
    close(clifd);
    return 0;
}



/**
 * @brief 与服务端建立连接
 */
int Connect(const char* ip,uint16_t port)
{
    int clifd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in srv_addr;
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(ip);
    srv_addr.sin_port = htons(port);
    
    int ret = connect(clifd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
    if (ret == 0) 
    {
        perror("成功与服务端建立连接");
    } 
    else 
    {
        perror("与服务端连接失败");
        return -1;
    }

    return clifd;
}


/**
 * @brief 处理客户端消息
 * @param arg 描述符地址
 */
void *recv_handler(void *arg) 
{
    int sockfd = *((int *)arg);
    unsigned char srv_msg[1024];

    while (true) 
    {
        ssize_t ret = read(sockfd, srv_msg, sizeof(srv_msg));
        if (ret > 0) 
        {
            AES_ECB_decrypt(srv_msg,decrypted,key);
            decrypted[1023]='\0';
            
            //判断认证是否成功
            if(strcmp((char*)decrypted,"认证失败！")==0)
            {
                cout<<"服务端："<<decrypted<<endl;
                exit(-1);
            }

            cout << "服务端: " << decrypted << endl;
            cout<<"请输入："<<flush;
        }
    }
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
     * @brief AES加密
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
     * @brief 设置解密密钥
     * @param key 密钥
     * @param aes_key AES_KEY对象指针
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
