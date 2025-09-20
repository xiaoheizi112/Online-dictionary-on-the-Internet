#ifndef _SE_H_
#define _SE_H_

#include "common.h"
#include <signal.h>
#include <sched.h>
#include <sys/wait.h>

// 处理用户注册请求
// @param db: 数据库句柄
// @param acceptfd: 客户端连接文件描述符
// @param msg: 消息结构体，包含用户名和密码
void handle_register(sqlite3 *db, int acceptfd, MSG *msg);

// 处理用户登录请求
// @param db: 数据库句柄
// @param acceptfd: 客户端连接文件描述符
// @param msg: 消息结构体，包含用户名和密码
void handle_login(sqlite3 *db, int acceptfd, MSG *msg);

// 查找单词定义
// @param acceptfd: 客户端连接文件描述符
// @param msg: 消息结构体，包含要查询的单词
// @return: 找到单词返回1，否则返回0
int handle_searchword(int acceptfd, MSG *msg);

// 处理单词查询请求并记录历史
// @param db: 数据库句柄
// @param acceptfd: 客户端连接文件描述符
// @param msg: 消息结构体，包含要查询的单词
void handle_query(sqlite3 *db, int acceptfd, MSG *msg);

// 检查用户查询历史
// @param db: 数据库句柄
// @param acceptfd: 客户端连接文件描述符
// @param msg: 消息结构体，包含用户名
void check_history(sqlite3 *db, int acceptfd, MSG *msg);

// 查询历史记录的回调函数
// @param arg: 用户数据（这里是客户端连接文件描述符）
// @param f_num: 字段数量
// @param f_value: 字段值数组
// @param f_name: 字段名数组
// @return: 0表示成功，非0表示失败
int history_callback(void *arg, int f_num, char **f_value, char **f_name);

#endif
