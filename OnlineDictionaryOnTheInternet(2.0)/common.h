#ifndef _COMMON_H_
#define _COMMON_H_

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
#include <time.h>
#include <error.h>

// 定义最大数据大小
#define MAX_SIZE 1024

// 消息类型宏定义
#define R 1  // 注册请求
#define L 2  // 登录请求
#define Q 3  // 查询单词请求
#define H 4  // 查询历史记录请求

// 消息结构体
// 用于客户端和服务器之间的通信
typedef struct MSG {
    int type;       // 消息类型，取值为R, L, Q, H
    char name[32];  // 用户名
    char data[256]; // 数据，根据消息类型不同，可以是密码或单词
} MSG;

#endif