#ifndef _CL_H_
#define _CL_H_

#include "common.h"

// 菜单选项枚举类型
enum CHOICE
{
    REGISTER = 1,  // 注册选项
    LOGIN,         // 登录选项
    QUIT1,         // 退出程序选项
    WORD,          // 查询单词选项
    HISTORY,       // 查询历史记录选项
    QUIT2          // 返回上一级菜单选项
};

// 全局消息结构体，用于存储登录用户信息
extern MSG msg_1;

// 显示主菜单
void menu1();

// 显示查询菜单（登录后）
void menu2();

// 处理用户注册请求
// @param sockfd: 套接字文件描述符
// @param msg: 消息结构体，包含注册信息
void handle_Register(int sockfd, MSG *msg);

// 处理用户登录请求
// @param sockfd: 套接字文件描述符
// @param msg: 消息结构体，包含登录信息
// @param msg_1: 全局消息结构体，用于存储登录后的用户信息
// @return: 登录成功返回1，失败返回0
int handle_login(int sockfd,MSG *msg,MSG *msg_1);

// 处理单词查询请求
// @param sockfd: 套接字文件描述符
// @param msg: 消息结构体，包含要查询的单词
void handle_query(int sockfd,MSG *msg); 

// 处理历史记录查询请求
// @param sockfd: 套接字文件描述符
// @param msg: 消息结构体
// @param msg_1: 全局消息结构体，包含登录用户信息
void handle_history(int sockfd,MSG *msg,MSG *msg_1);
#endif
