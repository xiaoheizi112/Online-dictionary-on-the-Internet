/**
 * @file cl.h
 * @brief 在线词典客户端核心功能头文件
 * 
 * 定义了客户端所需的数据结构、常量、枚举和函数声明，
 * 包括用户界面菜单、注册、登录、单词查询和历史记录等功能。
 */
#ifndef _CL_H_
#define _CL_H_

// 标准C库头文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <error.h>

// Linux系统调用头文件
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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
 * @brief 用户菜单选择枚举
 * 
 * 定义客户端界面中的所有可能操作选项
 */
enum CHOICE
{
    REGISTER = 1,    ///< 注册选项
    LOGIN,           ///< 登录选项
    QUIT1,           ///< 退出选项（主菜单）
    WORD,            ///< 查询单词选项
    HISTORY,         ///< 查询历史记录选项
    QUIT2            ///< 退出选项（登录后菜单）
};

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
}MSG;                            ///< 类型重定义，简化使用

// 全局变量声明
extern MSG msg_1;                ///< 全局消息结构体变量

// 函数声明

/**
 * @brief 显示主菜单
 * 
 * 显示注册、登录和退出选项的主界面
 */
void menu1();

/**
 * @brief 显示登录后菜单
 * 
 * 显示查询单词、查看历史记录和退出选项的界面
 */
void menu2();

/**
 * @brief 处理用户注册
 * 
 * 收集用户输入的注册信息并发送到服务器
 * 
 * @param sockfd 客户端套接字描述符
 * @param msg 消息结构体，用于存储和发送注册信息
 */
void handle_Register(int sockfd, MSG *msg);
/**
 * @brief 处理用户登录
 * 
 * 收集用户输入的登录信息并发送到服务器进行验证
 * 
 * @param sockfd 客户端套接字描述符
 * @param msg 消息结构体，用于存储和发送登录信息
 * @param msg_1 全局消息结构体，用于存储登录状态
 * @return 登录成功返回1，失败返回0
 */
int handle_login(int sockfd, MSG *msg, MSG *msg_1);

/**
 * @brief 处理单词查询
 * 
 * 收集用户输入的单词并发送到服务器进行查询
 * 
 * @param sockfd 客户端套接字描述符
 * @param msg 消息结构体，用于存储和发送查询信息
 */
void handle_query(int sockfd, MSG *msg); 

/**
 * @brief 处理历史记录查询
 * 
 * 向服务器请求并接收当前用户的查询历史记录
 * 
 * @param sockfd 客户端套接字描述符
 * @param msg 消息结构体
 * @param msg_1 全局消息结构体，包含当前登录用户信息
 */
void handle_history(int sockfd, MSG *msg, MSG *msg_1);

#endif  // _CL_H_
