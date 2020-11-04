#include<stdio.h>
#include <time.h>
#include <windows.h>

#pragma warning(disable:4996)

# define NOT(a) (a==0?1:0)

int twinkle = 1;



void print_11(int num, int size)
{
	printf_s(" ");
	if (!(num == 1 || num == 4))
		for (int i = size; i > 0; i--)
			printf_s("-");
	else
		for (int i = size; i > 0; i--)
			printf_s(" ");
	printf_s(" ");
}

void print_1(int* time, int size)
{
	
	print_11(time[0], size);
	printf_s(" ");
	print_11(time[1], size);
	printf_s("         ");//space x 9
	print_11(time[2], size);
	printf_s(" ");
	print_11(time[3], size);
	printf_s("         ");//space x 9
	print_11(time[4], size);
	printf_s(" ");
	print_11(time[5], size);

}

void print_21(int num, int size)
{
	if (num == 0 || num == 4 || num == 5 || num == 6 || num == 8 || num == 9)//第一个竖线
		printf_s("|");
	else
		printf_s(" ");

	for (int i = size; i > 0; i--)//两个竖线间的空格
		printf_s(" ");

	if (num == 0 || num == 1 || num == 2 || num == 3 || num == 4 || num == 7 || num == 8 || num == 9)//第二个竖线
		printf_s("|");
	else
		printf_s(" ");
}

void print_2(int* time, int size)
{
	for (int i = size; i > 0; i--)//高度为size
	{
		print_21(time[0], size);
		printf_s(" ");
		print_21(time[1], size);
		if(twinkle ==0)
			printf_s("         ");//space x 9
		else
			printf_s("    *    ");//space x 8
		print_21(time[2], size);
		printf_s(" ");
		print_21(time[3], size);
		if (twinkle == 0)
			printf_s("         ");//space x 9
		else
			printf_s("    *    ");//space x 8
		print_21(time[4], size);
		printf_s(" ");
		print_21(time[5], size);
		printf_s("\n");

	}

}


void print_31(int num, int size)
{
	printf_s(" ");
	if (!(num == 0 || num == 1 || num == 7))
		for (int i = size; i > 0; i--)
			printf_s("-");
	else
		for (int i = size; i > 0; i--)
			printf_s(" ");
	printf_s(" ");
}

void print_3(int* time, int size)
{
	print_31(time[0], size);
	printf_s(" ");
	print_31(time[1], size);
	printf_s("         ");//space x 9
	print_31(time[2], size);
	printf_s(" ");
	print_31(time[3], size);
	printf_s("         ");//space x 9
	print_31(time[4], size);
	printf_s(" ");
	print_31(time[5], size);

}



void print_41(int num, int size)
{
	if (num == 0 || num == 2 || num == 6 || num == 8 )//第一个竖线
		printf_s("|");
	else
		printf_s(" ");

	for (int i = size; i > 0; i--)//两个竖线间的空格
		printf_s(" ");

	if (num == 0 || num == 1 || num == 3 || num == 4 || num == 5 || num == 6 || num == 7 || num == 8 || num == 9)//第二个竖线
		printf_s("|");
	else
		printf_s(" ");
}

void print_4(int* time, int size)
{
	for (int i = size; i > 0; i--)//高度为size
	{
		print_41(time[0], size);
		printf_s(" ");
		print_41(time[1], size);
		if (twinkle == 0)
			printf_s("         ");//space x 9
		else
			printf_s("    *    ");//space x 8
		print_41(time[2], size);
		printf_s(" ");
		print_41(time[3], size);
		if (twinkle == 0)
			printf_s("         ");//space x 9
		else
			printf_s("    *    ");//space x 8
		print_41(time[4], size);
		printf_s(" ");
		print_41(time[5], size);
		printf_s("\n");

	}
}

void print_51(int num, int size)
{
	printf_s(" ");
	if (!(num == 1 || num == 4 || num == 7))
		for (int i = size; i > 0; i--)
			printf_s("-");
	else
		for (int i = size; i > 0; i--)
			printf_s(" ");
	printf_s(" ");
}

void print_5(int* time, int size)
{
	print_51(time[0], size);
	printf_s(" ");
	print_51(time[1], size);
	printf_s("         ");//space x 9
	print_51(time[2], size);
	printf_s(" ");
	print_51(time[3], size);
	printf_s("         ");//space x 9
	print_51(time[4], size);
	printf_s(" ");
	print_51(time[5], size);
}


void print_time(int* time,int size)
{
	print_1(time, size);
	printf_s("\n");
	print_2(time, size);
	print_3(time, size);
	printf_s("\n");
	print_4(time, size);
	print_5(time, size);
}

int main(void)
{

	time_t nowtime;
	nowtime = time(NULL); //获取日历时间

	struct tm* local, * gm;
	
	

	
	printf_s("输入时钟大小：\n");
	int size = 0;
	scanf_s("%d", &size);


	while (1)
	{
		nowtime = time(NULL); //获取日历时间
		local = localtime(&nowtime);  //获取当前系统时间
		int time[6];
		int hour = local->tm_hour;
		int mins = local->tm_min;
		int secs = local->tm_sec;
		if (hour < 10)
		{
			time[0] = 0;
			time[1] = hour;
		}
		else
		{
			time[0] = hour / 10;
			time[1] = hour % 10;
		}

		if (mins < 10)
		{
			time[2] = 0;
			time[3] = mins;
		}
		else
		{
			time[2] = mins / 10;
			time[3] = mins % 10;
		}

		if (secs < 10)
		{
			time[4] = 0;
			time[5] = secs;
		}
		else
		{
			time[4] = secs / 10;
			time[5] = secs % 60 % 10;
		}
		print_time(time, size);

		Sleep(460);
		system("cls");

		twinkle = NOT(twinkle);
	}


	
	return 0;
}
