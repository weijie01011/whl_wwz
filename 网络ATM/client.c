#include <stdio.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <string.h>
#include <sys/msg.h>
#include "struct.h"
#include "display.h"
#include "tools.h"
#include "key.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*显示子操作界面*/
void c_dis(int id,int fd);
/*显示主操作界面*/
void m_dis(int fd);

void biubiubiu(void)
{
	system("clear");
	printf("---------正在登录系统---------\n");
	int fd1 = open("1",O_RDWR,00644);
	char buf[1000] = {};
	read(fd1,buf,sizeof(buf));
	close(fd1);
	printf("%s",buf);
	sleep(1);
	system("clear");
	printf("---------正在登录系统---------\n");
	int fd2 = open("2",O_RDWR,00644);
	read(fd2,buf,sizeof(buf));
	close(fd2);
	printf("%s",buf);
	sleep(1);
	printf("\n");
}

typedef struct sockaddr SockAddr;

/*开户*/
void atm_creat(int fd)
{
	clear_scr();
	char recv_buf[1024];
	Acc acc = {};
	char money[14] = {};
	/*输入姓名,身份证,密码,预存金额*/
	printf("请输入姓名:");
	scanf("%s",acc.name);
	if(strlen(acc.name) > 20)
	{
		printf("输入违法!\n");
		sleep(1);
		return;
	}
	acc.name[20] = '\0';
	printf("请输入身份证:");
	while(NULL == get_lim(acc.identify,20));
	printf("请输入密码:");
	while(NULL == get_key_lim(acc.password,7));
	printf("请输入预存金额:");
	while(NULL == get_lim(money,14));
	acc.money = atof(money);
	acc.res = MY_CREAT;
	memset(recv_buf,0,1024);
	// 发送消息给服务器
	send(fd,&acc,sizeof(Acc),0);
	// 等待接受服务端的反馈
	recv(fd,recv_buf,1024,0);
	puts(recv_buf);
	/*按任意键返回*/
	press_anykey();
}
/*登录验证*/
void load(int fd)
{
	char recv_buf[1024];
	Acc acc = {};
	char ID[10] = {};
	/*输入账号,密码*/
	printf("账号:");
	while(NULL == get_lim(ID,10));
	acc.id = atoi(ID);
	printf("密码:");
	while(NULL == get_key_lim(acc.password,7));
	acc.res = MY_LOAD;
	memset(recv_buf,0,1024);
	// 发送消息给服务器
	send(fd,&acc,sizeof(Acc),0);
	// 等待接受服务端的反馈
	recv(fd,recv_buf,1024,0);
	if(0 == strcmp(recv_buf,"登录成功"))
	{
		show_msg(recv_buf,1);
		c_dis(acc.id,fd);
	}
	else
	{
		show_msg(recv_buf,1);
	}
}
/*存钱*/
void save(int id,int fd)
{
	char recv_buf[1024];
	Acc acc = {};
	acc.id = id;
	char money[14] = {};
	/*输入存入的金额*/
	printf("请输入金额:");
	while(NULL == get_lim(money,14));
	acc.money = atof(money);
	acc.res = MY_SAVE;
	memset(recv_buf,0,1024);
	// 发送消息给服务器
	send(fd,&acc,sizeof(Acc),0);
	// 等待接受服务端的反馈
	recv(fd,recv_buf,1024,0);
	puts(recv_buf);
	/*按任意键返回*/
	press_anykey();
}

/*取钱*/
void take(int id,int fd)
{
	char recv_buf[1024];
	Acc acc = {};
	acc.id = id;
	char money[14] = {};
	/*输入取出的金额*/
	printf("请输入金额:");
	while(NULL == get_lim(money,14));
	acc.money = atof(money);
	acc.res = MY_TAKE;
	memset(recv_buf,0,1024);
	// 发送消息给服务器
	send(fd,&acc,sizeof(Acc),0);
	// 等待接受服务端的反馈
	recv(fd,recv_buf,1024,0);
	puts(recv_buf);
	/*按任意键返回*/
	press_anykey();
}

/*查询余额*/
void query(int id,int fd)
{
	char recv_buf[1024];
	Acc acc = {};
	acc.id = id;
	acc.res = MY_QUERY;
	memset(recv_buf,0,1024);
	// 发送消息给服务器
	send(fd,&acc,sizeof(Acc),0);
	// 等待接受服务端的反馈
	recv(fd,recv_buf,1024,0);
	puts(recv_buf);
	/*按任意键返回*/
	press_anykey();
}
/*转账*/
void trans(int id,int fd)
{
	char recv_buf[1024];
	Tra tra = {};
	tra.id_my = id;
	char money[14] = {};
	char ID[10] = {};
	/*输入转账对象,金额*/
	printf("转账对象:");
	while(NULL == get_lim(ID,10));
	tra.id_obj = atoi(ID);
	printf("金额:");
	while(NULL == get_lim(money,14));
	tra.money = atof(money);
	if(id == tra.id_obj)
	{
		printf("转账对象不能为本账户\n");
		press_anykey();
		return;
	}
	tra.res = MY_TRANS;
	memset(recv_buf,0,1024);
	// 发送消息给服务器
	send(fd,&tra,sizeof(Tra),0);
	// 等待接受服务端的反馈
	recv(fd,recv_buf,1024,0);
	puts(recv_buf);
	/*按任意键返回*/
	press_anykey();
}

/*修改密码*/
void modify(int id,int fd)
{
	char recv_buf[1024];
	Mod mod = {};
	mod.id = id;
	char new1[7] = {};
	char new2[7] = {};
	/*输入旧密码,新密码*/
	printf("旧密码:");
	while(NULL == get_key_lim(mod.password_old,7));
	printf("新密码:");
	while(NULL == get_key_lim(new1,7));
	printf("再一次输入:");
	while(NULL == get_key_lim(new2,7));
	/*对比两次输入是否相同*/
	if(0 != strcmp(new1,new2))
	{
		printf("两次密码输入不同\n");
		press_anykey();
		return;
	}
	strcpy(mod.password_new,new1);
	/*对比旧密码和新密码是否相同*/
	if(0 == strcmp(mod.password_old,mod.password_new))
	{
		printf("新密码不能与旧密码重复\n");
		press_anykey();
		return;
	}
	mod.res = MY_MODIFY;
	memset(recv_buf,0,1024);
	// 发送消息给服务器
	send(fd,&mod,sizeof(Mod),0);
	// 等待接受服务端的反馈
	recv(fd,recv_buf,1024,0);
	puts(recv_buf);
	press_anykey();
}
/*解锁*/
void unlock(int fd)
{
	clear_scr();
	char recv_buf[1024];
	Acc acc = {};
	char ID[10] = {};
	/*输入账号,身份证*/
	printf("输入账号:");
	while(NULL == get_lim(ID,10));
	acc.id = atoi(ID);
	printf("输入身份证号:");
	while(NULL == get_lim(acc.identify,20));
	printf("输入新密码:");
	while(NULL == get_key_lim(acc.password,7));
	acc.res = UNLOCK;
	memset(recv_buf,0,1024);
	// 发送消息给服务器
	send(fd,&acc,sizeof(Acc),0);
	// 等待接受服务端的反馈
	recv(fd,recv_buf,1024,0);
	puts(recv_buf);
	press_anykey();
}
/*销户*/
void destory(int id,int fd)
{
	char recv_buf[1024];
	Acc acc = {};
	acc.id = id;
	char ch = 0;
	/*输入密码,并再次确认是否销户*/
	printf("输入密码:");
	while(NULL == get_key_lim(acc.password,7));
	printf("是否销户(y/n)\n");
	ch = y_or_n();
	if(ch == 'n' || ch == 'N')
		return;
	acc.res = MY_DESTORY;
	memset(recv_buf,0,1024);
	// 发送消息给服务器
	send(fd,&acc,sizeof(Acc),0);
	// 等待接受服务端的反馈
	recv(fd,recv_buf,1024,0);
	if(0 == strcmp(recv_buf,"销户成功"))
	{
		show_msg(recv_buf,1);
		m_dis(fd);
	}
	else
	{
		show_msg(recv_buf,1);
	}
}
/*退出在线状态*/
void atm_exit(int id,int fd)
{
	char recv_buf[1024];
	Acc acc = {};
	acc.id = id;
	acc.res = MY_EXIT;
	memset(recv_buf,0,1024);
	// 发送消息给服务器
	send(fd,&acc,sizeof(Acc),0);
}
/*显示子操作界面*/
void c_dis(int id,int fd)
{
	biubiubiu();
	while(1)
	{
		clear_scr();
		c_display();
		switch(get_cmd('1','7'))
		{
			case '1':save(id,fd);	    break;//存钱
			case '2':take(id,fd); 		break;//取钱
			case '3':query(id,fd);		break;//查询余额
			case '4':trans(id,fd);		break;//转账
			case '5':modify(id,fd);		break;//修改密码
			case '6':destory(id,fd);	break;//销户
			case '7':atm_exit(id,fd);	return;	//退出
		}
	}
}
/*显示主界面*/
void m_dis(int fd)
{
	biubiubiu();
	while(1)
	{
		clear_scr();
		m_display();
		switch(get_cmd('1','4'))
		{
			case '1':atm_creat(fd);		 break;//开户
			case '2':load(fd);		 break;//登入
			case '3':unlock(fd);	 break;//解锁
			case '4':return;//退出
		}
	}
}

void* run(void* sockfd)
{
	int fd = (int)sockfd;
	m_dis(fd);
	exit(0);
}

int main(void)
{
	// 创建socket对象
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(0 > sockfd)
	{
		perror("socket");
		return -1;
	}

	// 准备通信地址
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(1999);
	addr.sin_addr.s_addr = inet_addr("192.168.1.121");

	// 连接
	if(0 > connect(sockfd,(SockAddr*)&addr,sizeof(addr)))
	{
		perror("connect");
		return -1;
	}
	
	// 创建线程
	pthread_t id;

	// 启动客户端功能
	pthread_create(&id,NULL,run,(void*)sockfd);
	
	// 等待线程结束,退出客户端
	pthread_join(id,NULL);
	return 0;
}
