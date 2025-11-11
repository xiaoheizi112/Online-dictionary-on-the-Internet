/**
 * @file se.h
 * @brief 在线词典服务器端核心功能头文件
 * 
 * 定义了服务器端所需的数据结构、常量和函数声明，
 * 包括用户注册、登录、单词查询和历史记录等功能。
 */
#ifndef _SE_H_
#define _SE_H_

// 标准C库头文件
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <error.h>

// Linux系统调用头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sched.h>
#include <signal.h>

// 网络编程相关头文件
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <sys/epoll.h>

// SQLite3数据库头文件
#include <sqlite3.h>

// 常量定义
#define MAX_SIZE 1024              ///< 最大字符串长度
#define R 1                        ///< 注册请求类型
#define L 2                        ///< 登录请求类型
#define Q 3                        ///< 单词查询请求类型
#define H 4                        ///< 历史记录查询请求类型

/**
 * @brief 消息结构体
 * 
 * 用于客户端与服务器之间通信的数据结构，
 * 包含消息类型、用户名和数据内容。
 */
typedef struct MSG
{
    int type;                     ///< 消息类型（R、L、Q、H）
    char name[32];                ///< 用户名
    char data[256];               ///< 数据内容（密码或单词）
}MSG;                             ///< 类型重定义，简化使用

// 函数声明

/**
 * @brief 处理用户注册请求
 * @param db SQLite数据库连接指针
 * @param acceptfd 客户端连接的套接字描述符
 * @param msg 消息结构体，包含用户名和密码
 */
void handle_register(sqlite3 *db, int acceptfd, MSG *msg);

/**
 * @brief 处理用户登录请求
 * @param db SQLite数据库连接指针
 * @param acceptfd 客户端连接的套接字描述符
 * @param msg 消息结构体，包含用户名和密码
 */
void handle_login(sqlite3 *db, int acceptfd, MSG *msg);

/**
 * @brief 在词典文件中查找单词
 * @param acceptfd 客户端连接的套接字描述符
 * @param msg 消息结构体，包含要查询的单词
 * @return 找到返回1，未找到返回0
 */
int handle_searchword(int acceptfd, MSG *msg);

/**
 * @brief 处理单词查询请求，并保存查询历史
 * @param db SQLite数据库连接指针
 * @param acceptfd 客户端连接的套接字描述符
 * @param msg 消息结构体，包含要查询的单词
 */
void handle_query(sqlite3 *db, int acceptfd, MSG *msg);

/**
 * @brief 查询用户的历史记录
 * @param db SQLite数据库连接指针
 * @param acceptfd 客户端连接的套接字描述符
 * @param msg 消息结构体，包含用户名
 */
void check_history(sqlite3 *db, int acceptfd, MSG *msg);

/**
 * @brief SQLite查询回调函数
 * @param arg 用户自定义参数，通常是套接字描述符
 * @param f_num 结果字段数量
 * @param f_value 结果值数组
 * @param f_name 结果字段名数组
 * @return 0表示继续处理，非0表示停止处理
 */
int history_callback(void *arg, int f_num, char **f_value, char **f_name);

#endif  // _SE_H_
