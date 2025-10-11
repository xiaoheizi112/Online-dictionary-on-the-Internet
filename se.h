#ifndef _SE_H_
#define _SE_H_

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
#include <signal.h>
#include <sched.h>
#include <sys/wait.h>
#include <error.h>

#define MAX_SIZE 1024

#define R 1
#define L 2
#define Q 3
#define H 4

typedef struct MSG
{
    int type;
    char name[32];
    char data[256];//password or word
}MSG;

//注册
void handle_register(sqlite3 *db,int acceptfd,MSG *msg);
//登录
void handle_login(sqlite3 *db,int acceptfd,MSG *msg);
//查找单词
int handle_searchword(int acceptfd,MSG *msg);
//查询
void handle_query(sqlite3 *db,int acceptfd,MSG *msg);
//查询历史记录
void check_history(sqlite3 *db,int acceptfd,MSG *msg);
//回调函数
int history_callback(void *arg, int f_num, char **f_value, char **f_name);

#endif
