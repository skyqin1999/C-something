#include <WINSOCK2.H>
#include <STDIO.H>
#include <windows.h>
#include <process.h>
#include <time.h>
#pragma warning(disable:4996)
#pragma  comment(lib,"ws2_32.lib")
#include <conio.h>



int getsys_time()
{
    time_t now;
    time(&now);// 等同于now = time(NULL)
    return now;
}


int a = 0; //全局变量
char msg[100] = "hello";
int cnt = 5;


int OA = 1;
int IA = 1;


HANDLE hout = GetStdHandle(STD_OUTPUT_HANDLE);
COORD coord;
COORD cur_tmp;
CONSOLE_SCREEN_BUFFER_INFO csbi;	//控制台屏幕缓冲区信息

constexpr auto HISTORY_NUM = 8;
constexpr auto HISTORY_MAX = 255;
constexpr auto BUFFER_MAX = 100;

char HISTORY[HISTORY_NUM][HISTORY_MAX] = { "\0" };//must by '\0'

char BUFFER[BUFFER_MAX] = "\0";

int get_time()
{
    time_t now;
    time(&now);// 等同于now = time(NULL)
    return now;
}
SOCKET SCLIENT;
int STATUS = 0;
int fd = -1;
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





void add2buffer(char c)
{
    int len = strlen(BUFFER);
    BUFFER[len] = c;
    BUFFER[BUFFER_MAX-1] = '\0';
}


void add2history_by_string(char* s)
{

    for (int i = 0; i < HISTORY_NUM - 1; i++)
        strcpy(HISTORY[i], HISTORY[i + 1]);

    strcpy(HISTORY[HISTORY_NUM - 1], s);

}

void add2buffer_by_string( char* s)
{
    int len = strlen(s);
    for (int i = 0; i < len; i++)
    {
        add2buffer(s[i]);
    }
}


void marge_print()
{
    system("cls");
    for (int i = 0; i < HISTORY_NUM; i++)
        printf("%s", HISTORY[i]);

    printf("%s", BUFFER);
}


void print_front_line(const char *s)
{

    while (!OA);

    GetConsoleScreenBufferInfo(hout, &csbi);	//获取屏幕缓冲的光标位置,然后把当前光标位置赋给COORD
    cur_tmp.X = coord.X = csbi.dwCursorPosition.X;
    cur_tmp.Y = coord.Y = csbi.dwCursorPosition.Y;
    cur_tmp.X = 0;
    cur_tmp.Y -= 1;

    SetConsoleCursorPosition(hout, cur_tmp);
    printf("%s", s);

    SetConsoleCursorPosition(hout, coord);
    
}






void back_space(int len)
{
    for (int i = 0; i < len; i++)
        printf("\b");
    for (int i = 0; i < len; i++)
        printf(" ");
    for (int i = 0; i < len; i++)
        printf("\b");
}

void back_buffer()
{
    int len = strlen(BUFFER);

    if (BUFFER[len - 1] == '\b')
    {
        if ((unsigned char)BUFFER[len - 2] >> 7)//汉字
        {
            BUFFER[len - 1] = '\0';
            BUFFER[len - 2] = '\0';
            BUFFER[len - 3] = '\0';
        }
        else
        {
            BUFFER[len - 1] = '\0';
            BUFFER[len - 2] = '\0';
        }
    }

    if (BUFFER[len - 2] == -32 && BUFFER[len - 1] == 75)
    {
        BUFFER[len - 1] = '\0';
        BUFFER[len - 2] = '\0';
    }
}

void send()
{
    
    char send_msg[500] = {'\0'};
    itoa(get_time(), send_msg, 10);
    strcat(send_msg, "-N-");
    char client_fd[2] = { '\0' };
    itoa(fd, client_fd, 10);
    strcat(send_msg, client_fd);
    strcat(send_msg, "-");
    strcat(send_msg, BUFFER);



    /*发送数据的格式为"时间戳-指令-num-内容"*/
    send(SCLIENT, send_msg, strlen(send_msg), 0);
    
    //OA = 1;
}



void tying_or_not(void *)
{
    int len = 0;
    int len_his = 0;

    while (1)
    {
        len = strlen(BUFFER);
        if (len != len_his)
        {

            char send_msg[50] = { '\0' };
            itoa(get_time(), send_msg, 10);
            strcat(send_msg, "-I-");
            char client_fd[2] = { '\0' };
            itoa(fd, client_fd, 10);
            strcat(send_msg, client_fd);
            strcat(send_msg, "-");
            strcat(send_msg, "Typing");

            /*发送数据的格式为"时间戳-I-num-Typing"*/
            send(SCLIENT, send_msg, strlen(send_msg), 0);
            len_his = len;
        }
        else
        {
            char send_msg[50] = { '\0' };
            itoa(get_time(), send_msg, 10);
            strcat(send_msg, "-H-");
            char client_fd[2] = { '\0' };
            itoa(fd, client_fd, 10);
            strcat(send_msg, client_fd);
            strcat(send_msg, "-");
            strcat(send_msg, "Not typing");

            /*发送数据的格式为"时间戳-H-num-Not typing"*/
            send(SCLIENT, send_msg, strlen(send_msg), 0);
        }
        Sleep(1500);
    }


    
}

void send_msg_func(void*)
{

    char c;
    char msg[200] = "\0";


    while (1)
    {

        c = getch();
        add2buffer(c);


        OA = 0;
        back_space(strlen(BUFFER));
        back_buffer();
        printf("%s", BUFFER);
        

        if (c == '\r')//a line is end by \n
        {

            //add2buffer('\n');


            BUFFER[strlen(BUFFER)] = '\0';//move \r
            send();

            back_space(strlen(BUFFER));

            //marge_print();
            memset(BUFFER, '\0', sizeof(BUFFER));

        }
        OA = 1;
    }


}

void recv_msg_func(void*)
{
    char get_msg[500] = { '\0' };
    char content[500] = { '\0' };
    char time_o[20] = { '\0' };
    char name[30] = { '\0' };
    char command_history = 'H';//hang on
    char command;
    int ret = 1;

    while (STATUS)
    {
        //IA = 0;




        ret = recv(SCLIENT, get_msg, 499, 0);

        if (ret <= 0)
            STATUS = 0;
        else
        {
            command = get_msg[11];
            if (command == 'N')
            {
                for (int i = 0; i < 10; ++i)
                    time_o[i] = get_msg[i];
                time_o[10] = '\0';
                time_t t = atoi(time_o);
                struct tm* tm = localtime(&t);
                char ins = get_msg[11];

                int i = 13, j = 0;
                while (get_msg[i] != '-')
                    name[j++] = get_msg[i++];

                i++;

                char msg_1[500] = "\0";

                strcpy(content, (get_msg + i));



                back_space(strlen(BUFFER));
                print_front_line("                        ");

                printf("%s    %02d:%02d:%02d \n  %s\n\n", name, tm->tm_hour, tm->tm_min, tm->tm_sec, content);


                printf("\n");


                printf("%s", BUFFER);

            }
            else if (command == 'I')
            {
                print_front_line("This guy is typing.....");
                command_history = command;
            }
            else if (command == 'H'&& command_history == 'I')
            {
                print_front_line("                        ");
                command_history = command;
            }

            

        }

        memset(time_o, '\0', sizeof(time_o));
        memset(get_msg, '\0', sizeof(get_msg));
        memset(name, '\0', sizeof(name));
        memset(content, '\0', sizeof(content));
    }
    
}


int main() {

    printf("Please input your name\n");
    char name[30];
    scanf("%s", name);


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
    serAddr.sin_addr.S_un.S_addr = inet_addr("119.3.250.206");


    if (connect(SCLIENT, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
    {
        printf("connect error !");
        closesocket(SCLIENT);
        return 0;
    }

    STATUS = 1;
    printf("服务器连接成功!\n");

    send(SCLIENT, name, strlen(name), 0);

    char get_msg[455] = { '\0' };
    int ret = recv(SCLIENT, get_msg, 499, 0);
    get_msg[ret] = '\0';
    fd = get_msg[0] - '0';
    printf("%s", get_msg);



    _beginthread(send_msg_func, 0, NULL);
    _beginthread(recv_msg_func, 0, NULL);
    _beginthread(tying_or_not, 0, NULL);

    while (1) {
        

        Sleep(1000);
    }
    return 0;
}
