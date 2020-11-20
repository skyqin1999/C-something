/*******************************************/
/*参考：https://chouti.blog.csdn.net/article/details/106221085



/*******************************************/

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <time.h>


#pragma warning(disable:4996)
#pragma comment(lib,"ws2_32.lib")
#define MAX_NUM_CLIENT 10


struct ClientNode
{
    int num;
    char name[30];
    int status;
    SOCKET client;
    int p_num;
    SOCKET parter;
   
};


ClientNode CLIENT_LIST[MAX_NUM_CLIENT];
struct tm* TIME[MAX_NUM_CLIENT];
//int CLIENT_STATUS[MAX_NUM_CLIENT];



int getsys_time()
{
    time_t now;
    time(&now);// 等同于now = time(NULL)
    return now;
}

void trans_time(char* time,int num)
{
    int tm = atoi(time);
    time_t t = tm;
    TIME[num] = localtime(&t);
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


/*发送数据的格式为"时间戳-指令-对象名称-内容"*/

void send_with_time(SOCKET client, char ins, int poster, const char* msg)
{
    char post[500] = { '\0' };
    char s[25] = { '\0' };
    char i[2] = { '\0' };
    i[0] = ins;
    char name[30] = { '\0' };
    itoa(getsys_time(), s, 10);

    if (poster == -1)
        strcpy(name, "Server");
    else
        strcpy(name, CLIENT_LIST[poster].name);
    strcat(post, s);
    strcat(post, "-");
    strcat(post, i);
    strcat(post, "-");
    strcat(post, name);
    strcat(post, "-");
    strcat(post, msg);

    send(client, post, strlen(post), 0);//发送
}




int bridge_msg(int id)
{
    SOCKET source = CLIENT_LIST[id].client;
    SOCKET dest = CLIENT_LIST[id].parter;


    char msg[500] = {'\0'};
    int len = strlen(CLIENT_LIST[id].name);

    

    char command[300];
    int ret = recv(source, msg, 499, 0);//阻塞的接收信息

    while (ret && CLIENT_LIST[id].status == 2 && CLIENT_LIST[CLIENT_LIST[id].p_num].status == 2)
    {

        msg[ret] = 0x00;

        for (int i = ret; i >13; i--)//移出发送方的名字的空间
            msg[i + len - 1] = msg[i];

        for (int i = 0; i < len; i++)
        {
            msg[13 + i] = CLIENT_LIST[id].name[i];
        }

        
        send(dest, msg, 499, 0);//发送信息
        
        if(msg[11]=='N')
            send(source, msg, 499, 0);//回送信息

        strcpy(command, (msg + 14 + len));

        if (strstr(command, "/bye") == command)
        {
            send_with_time(source, 'N', -1, "You've releaed this chat line!");//发送信息
            send_with_time(dest, 'N', -1, "Bye, see you later!");//发送信息
            return 1;
        }

        memset(msg, '\0', sizeof(msg));
        memset(command, '\0', sizeof(command));

        ret = recv(source, msg, 255, 0);//阻塞的接收信息
    }

    return 0;
}


void show_all_usr(int num)
{
    char s[255] = { '\0' };

    strcat(s, "All users in this server is list follow:\n");
    int no = 0;
    char no_a[3];
    for (int i = 0; i < MAX_NUM_CLIENT; i++)
    {
        if (CLIENT_LIST[i].status != 0)
        {
            no++;
            strcat(s, "  ");
            itoa(i,no_a,10);
            strcat(s, no_a);
            strcat(s, ": ");
            strcat(s, CLIENT_LIST[i].name);
            strcat(s, "\n");
        }
    }

    if (no == 1)
    strcat(s, "\n  It seems that you're alone\n  Share this server to your friends!!\n");
    
    send_with_time(CLIENT_LIST[num].client,'N',-1, s);
}


void chat(int num)
{
    CLIENT_LIST[num].status = 2;//chat mode
    char chat_o[300] = { '\0' };
    char chat_i[300] = { '\0' };
    show_all_usr(num);
    strcat(chat_o, "Input the num of the guy you want to chat with:\n");
    send_with_time(CLIENT_LIST[num].client, 'N', -1, chat_o);

    int ret = recv(CLIENT_LIST[num].client, chat_i, 255, 0);//接收聊天对象的序号信息
    while (chat_i[11] != 'N')
        ret = recv(CLIENT_LIST[num].client, chat_i, 255, 0);//是否接受邀请的回复

    chat_i[ret] = '\0';

    char n[4] = { '\0' };
    strcpy(n, (chat_i + 15));

    int chat_to = atoi(n);

    if (CLIENT_LIST[chat_to].status == 1)
    {

        char invitation[100] = {'\0'};



        strcat(invitation, CLIENT_LIST[num].name);

        send_with_time(CLIENT_LIST[num].client, 'N', -1, "Wait for accept......\n");
        CLIENT_LIST[chat_to].status = 9;//ready to recv chat
        strcat(invitation, " invitate you to chat! \n* Enter '1' twice to accept & other to decline");
        send_with_time(CLIENT_LIST[chat_to].client, 'N', -1, invitation);//发送邀请信息


        memset(chat_i, '\0', sizeof(chat_i));
        ret = recv(CLIENT_LIST[chat_to].client, chat_i, 255, 0);//是否接受邀请的回复
        
        while(chat_i[11]!='N')
            ret = recv(CLIENT_LIST[chat_to].client, chat_i, 255, 0);//是否接受邀请的回复

        chat_i[ret] = '\0';

        if (chat_i[15] == '1')
        {
            CLIENT_LIST[chat_to].status = 2;
            CLIENT_LIST[chat_to].parter = CLIENT_LIST[num].client;
            CLIENT_LIST[chat_to].p_num = CLIENT_LIST[num].num;

            CLIENT_LIST[num].p_num = CLIENT_LIST[chat_to].num;
            CLIENT_LIST[num].parter = CLIENT_LIST[chat_to].client;

            send_with_time(CLIENT_LIST[num].client, 'N', -1, "This guy accept your invitation\n");
            send_with_time(CLIENT_LIST[chat_to].client, 'N', -1, "You have accepted this invitation\n");
            bridge_msg(num);
        }
        else
        {
            memset(invitation,'\0',sizeof(invitation));
            strcat(invitation, CLIENT_LIST[chat_to].name);
            strcat(invitation, " decline your invitation!!");
            send_with_time(CLIENT_LIST[num].client, 'N', -1, invitation);//回复邀请失败的信息
        }

        
    }
    else
    {
        send_with_time(CLIENT_LIST[num].client, 'N', -1, "This guy is busy\n");
    }
        
    CLIENT_LIST[num].status = 1;//normal mode
    CLIENT_LIST[chat_to].status = 1;//normal mode


}


void new_client(void* arg)
{
    int num = *(int*)arg;
    SOCKET client = CLIENT_LIST[num].client;

    char fd_num[4];
    itoa(num, fd_num, 10);
    char name[30];
    int ret = recv(client, name, 30, 0);//接收昵称
    name[ret] = '\0';

    strcpy(CLIENT_LIST[num].name, name);


    char welcome[455]="\0";
    
    strcat(welcome, fd_num);
    strcat(welcome, " - heiii  ");
    strcat(welcome, name);
    strcat(welcome, "\nwelcome to connect this server and your client id is :");
    strcat(welcome, fd_num);
    strcat(welcome, "\n**********************************************");
    strcat(welcome, "\n1. use  /quit  to disconnect");
    strcat(welcome, "\n2. use  /show_usrs  to show all user in server");
    strcat(welcome, "\n3. use  /chat  to select & chat with someone");
    //strcat(welcome, "\n4. use  /game  to select & play with someone");
    strcat(welcome, "\n**********************************************\n\n");
    send(client, welcome, strlen(welcome), 0);


    

    char msg[255] = { '\0' };
    //ret = recv(client, msg, 255, 0);//阻塞的接收信息
    int time = 0;
    char content[250];
    char time_o[20];


    while (1)
    {

        if (CLIENT_LIST[num].status == 1)//1:normal, 2:chat mode
        {

            ret = recv(client, msg, 255, 0);//阻塞的接收信息

            //need error tect here

            msg[ret] = 0x00;

            for (int i = 0; i < 10; ++i)
                time_o[i] = msg[i];
            time_o[10] = '\0';
            time_t t = atoi(time_o);
            TIME[num] = localtime(&t);

            strcpy(content, (msg + 15));

            printf("%s  %d:%d:%d \n  %s\n\n", name, TIME[num]->tm_hour, TIME[num]->tm_min, TIME[num]->tm_sec, content);


            if (strstr(content, "/quit") == content)
            {
                printf("client %d quit!!!\n", num);
                break;
            }
            else if (strstr(content, "/show_usrs") == content)
            {
                show_all_usr(num);
            }
            if (strstr(content, "/chat") == content)
            {
                chat(num);
            }
            
            
        }
        else if (CLIENT_LIST[num].status == 2)//1:normal, 2:chat mode
        {
            bridge_msg(num);
            //主动释放别人邀请的聊天
            CLIENT_LIST[num].status = 1;//设置自己状态为与服务器沟通
            CLIENT_LIST[CLIENT_LIST[num].p_num].status = 1;//设置对方状态为与服务器沟通
        }

        memset(msg, '\0', sizeof(msg));
        memset(content, '\0', sizeof(content));
        memset(time_o, '\0', sizeof(time_o));


        
            
    }

    CLIENT_LIST[num].status = 0;

    printf("close connection!!!\n");

    closesocket(client);

    return ;
}

void socket_all()
{

    for (int i = 0; i < MAX_NUM_CLIENT; i++)
        CLIENT_LIST[i].num = i;


    //创建套接字
    SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (slisten == INVALID_SOCKET)
    {
        printf("socket error !");
        return ;
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

    SOCKET client_fd;

    int client_num = 0;

    //开始监听
    if (listen(slisten, 5) == SOCKET_ERROR)
    {
        printf("listen error !");
        return;
    }

    //循环接收数据

    sockaddr_in remoteAddr;
    int nAddrlen = sizeof(remoteAddr);

    //聊天程序

    while (true)
    {
        printf("\n等待新的连接...\n");
        client_fd = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
        if (client_fd == INVALID_SOCKET)
        {
            printf("accept error !");
            continue;
        }
        printf("接受到一个连接：%s \r\n", inet_ntoa(remoteAddr.sin_addr));





        for (int i = 0; i < MAX_NUM_CLIENT; i++)
        {
            if (CLIENT_LIST[i].status == 0)
            {
                CLIENT_LIST[i].client = client_fd;
                CLIENT_LIST[i].status = 1;

                if (-1 == _beginthread(new_client, 0, &i))
                {
                    printf("client thread creat fail...\n");
                    break;
                }
                printf("client %d is join...\n", i);
                client_num++;
                if (client_num == MAX_NUM_CLIENT - 1)
                {
                    printf("client full...\n");
                }
                break;
            }
        }

    }

    closesocket(slisten);
    WSACleanup();
    return;
}



int main(int argc, char* argv[])
{
    socket_init();
    socket_all();
    Sleep(20000);
    while (1)
    {
        Sleep(1000);
    }
    
}

