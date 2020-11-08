#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <time.h>


#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")


SOCKET FIRST;
int FIRST_STAUS = 0;

SOCKET SECOND;
int SECOND_STAUS = 0;

struct MsgNode
{
    int time;
    char msg[255];
};

int getsys_time()
{
    time_t now;
    time(&now);// 等同于now = time(NULL)
    return now;
}



int socket_init()
{
    //初始化WSA
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (WSAStartup(sockVersion, &wsaData) != 0)
    {
        return 0;
    }
}

SOCKET socket_first_init()
{
   

    //创建套接字
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(8888);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }
    return slisten;
}

SOCKET socket_second_init()
{


    //创建套接字
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return 0;
    }

    //绑定IP和端口
    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(9999);
    sin.sin_addr.S_un.S_addr = INADDR_ANY;
    if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
    {
        printf("bind error !");
    }
    return slisten;
}



int bridge_msg(SOCKET source, SOCKET dest)
{
    char msg[255];
    int ret = recv(source, msg, 255, 0);//阻塞的接收信息

    while (ret)
    {

        msg[ret] = 0x00;
       
        send(dest, msg, 255, 0);//发送信息

        ret = recv(source, msg, 255, 0);//阻塞的接收信息

    }

    printf("ERROR!!!\n");

    return 0;
}


void socket_first(void*)
{
    SOCKET slisten = socket_first_init();

    //开始监听
    if (listen(slisten, 5) == SOCKET_ERROR)
    {
        printf("listen error !");
        return ;
    }

    //循环接收数据
    
    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);

    //聊天程序

    while (true)
    {
        printf("等待A的连接...\n");
        FIRST = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
        if (FIRST == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        printf("接受到A的连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));


        FIRST_STAUS = 1;
        

        if(FIRST_STAUS==1&&SECOND_STAUS==0)
            send(FIRST, "等待对方连接服务器", strlen("等待对方连接服务器\n"), 0);

        while (FIRST_STAUS)
        {
            
            if (SECOND_STAUS)
            {
                bridge_msg(FIRST, SECOND);
            }
        }


        printf("ERROR!!!\n");
        closesocket(FIRST);
    }

    closesocket(slisten);
    WSACleanup();
    return ;
}

void socket_second(void*)
{
    SOCKET slisten = socket_second_init();

    //开始监听
    if (listen(slisten, 5) == SOCKET_ERROR)
    {
        printf("listen error !");
        return ;
    }

    //循环接收数据

    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);

    //聊天程序

    while (true)
    {
        printf("等待B的连接...\n");
        SECOND = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
        if (SECOND == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        printf("接受到B的连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));


        SECOND_STAUS = 1;
        if (FIRST_STAUS == 0 && SECOND_STAUS == 1)
            send(FIRST, "等待对方连接服务器", strlen("等待对方连接服务器\n"), 0);
        while (SECOND_STAUS)
        {

            if (FIRST_STAUS)
            {
                bridge_msg(SECOND, FIRST);
            }
        }


        printf("ERROR!!!\n");
        closesocket(SECOND);
    }

    closesocket(slisten);
    WSACleanup();
    return ;
}



int main(int argc, char* argv[])
{
    socket_init();
    _beginthread(socket_first, 0, NULL);
    _beginthread(socket_second, 0, NULL);
    Sleep(20000);
    while (1)
    {
        if(!(FIRST_STAUS && SECOND_STAUS))
            printf("server die\n");
    }
    
}
