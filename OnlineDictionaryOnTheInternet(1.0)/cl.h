#ifndef _CL_H_
#define _CL_H_

#include <stdio.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include<time.h>
#include <error.h>

#define MAX_SIZE 1024

#define R 1
#define L 2
#define Q 3
#define H 4

enum CHOICE
{
    REGISTER = 1,
    LOGIN,
    QUIT1,
    WORD,
    HISTORY,
    QUIT2
};

typedef struct MSG
{
    int type;
    char name[32];
    char data[256];//password or word
}MSG;

extern MSG msg_1;
//菜单1
void menu1();
//菜单2
void menu2();
//注册
void handle_Register(int sockfd,MSG *msg);
//登录
int handle_login(int sockfd,MSG *msg,MSG *msg_1);
//查询单词
void handle_query(int sockfd,MSG *msg); 
// 查询历史记录
void handle_history(int sockfd,MSG *msg,MSG *msg_1);
#endif
