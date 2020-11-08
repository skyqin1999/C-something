
#include <WINSOCK2.H>
#include <STDIO.H>
#include <windows.h>
#include <process.h>
#include <time.h>
#pragma warning(disable:4996)
#pragma  comment(lib,"ws2_32.lib")

int getsys_time()
{
    time_t now;
    time(&now);// 等同于now = time(NULL)
    return now;
}
SOCKET SCLIENT;
int STATUS = 0;
char SEND_MSG[255];

int socket_init()
{
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA data;
    if (WSAStartup(sockVersion, &data) != 0)
    {
        return 0;
    }
}


void recv_msg_func(void*)
{
    char get_msg[255];
    memset(get_msg, '\0', sizeof(get_msg));

    int ret = 1;

    while (STATUS)
    {
        ret = recv(SCLIENT, get_msg, 255, 0);

        if (ret <= 0)
            STATUS = 0;
        else
            printf("%s", get_msg);
    }
}

void send_msg_func(void*)
{
    char sen_msg[255];
    memset(sen_msg, '\0', sizeof(sen_msg));

    while (STATUS)
    {
        scanf("%s", sen_msg);
        printf("\n");
        //发送时间戳
        char tm[11];
        itoa(getsys_time(), tm, 10);


        //发送输入的信息
        send(SCLIENT, "用户：A      时间：", strlen("用户：B      时间："), 0);
        send(SCLIENT, tm, strlen(tm), 0);
        send(SCLIENT, ": \n", 3 * sizeof(char), 0);
        send(SCLIENT, sen_msg, strlen(sen_msg), 0);
        send(SCLIENT, "\n\n", 2 * sizeof(char), 0);

        memset(sen_msg, '\0', sizeof(sen_msg));
    }

}



int main(int argc, char* argv[])
{

    socket_init();
    SCLIENT = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (SCLIENT == INVALID_SOCKET)
    {
        printf("invalid socket !");
        return 0;
    }

    sockaddr_in serAddr;
    serAddr.sin_family = AF_INET;
    serAddr.sin_port = htons(8888);
    serAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//本地测试使用


    if (connect(SCLIENT, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
        printf("connect error !");
        closesocket(SCLIENT);
        return 0;
    }

    STATUS = 1;
    printf("服务器连接成功!\n");

    _beginthread(recv_msg_func, 0, NULL);
    _beginthread(send_msg_func, 0, NULL);

    Sleep(6000);
    while (1)
    {
        //printf("STATUS: %d\n", STATUS);
        Sleep(3000);
    }



    printf("中断连接!!!\n");
    closesocket(SCLIENT);

    closesocket(SCLIENT);
    WSACleanup();
    return 0;
}
