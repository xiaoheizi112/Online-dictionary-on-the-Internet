#include"cl.h"
// 显示主菜单
// 包含注册、登录和退出选项
void menu1()
{
    printf("*************菜单**************\n");
    printf("* 1.注册                    *\n");
    printf("* 2.登录                    *\n");
    printf("* 3.退出                    *\n");
    printf("******************************\n");
}

// 显示查询菜单
// 登录成功后显示，包含查询单词、查询历史记录和退出选项
void menu2()
{
    printf("*************菜单**************\n");
    printf("* 4.查询单词                *\n");
    printf("* 5.查询历史记录            *\n");
    printf("* 6.退出                    *\n");
    printf("******************************\n");
}

// 处理用户注册请求
// @param sockfd: 套接字文件描述符
// @param msg: 消息结构体，用于存储注册信息
void handle_Register(int sockfd, MSG *msg)
{
    // 设置消息类型为注册请求
    msg->type = R;

    // 获取用户输入的注册信息
    printf("请输入用户信息:\n");
    printf("请输入用户名:");
    scanf("%s", msg->name);
    printf("请输入密码:");
    scanf("%s", msg->data);

    // 发送注册请求到服务器
    send(sockfd, msg, sizeof(MSG), 0);

    // 接收服务器响应
    recv(sockfd, msg, sizeof(MSG), 0);

    // 根据响应结果显示注册成功或失败
    if (strcmp(msg->data, "OK") == 0)
    {
        printf("注册成功\n");
    }
    else
    {
        printf("注册失败: %s\n", msg->data);
    }
}

// 处理用户登录请求
// @param sockfd: 套接字文件描述符
// @param msg: 消息结构体，用于存储登录信息
// @param msg_1: 全局消息结构体，用于存储登录后的用户信息
// @return: 登录成功返回1，失败返回0
int handle_login(int sockfd, MSG *msg, MSG *msg_1)
{
    // 设置消息类型为登录请求
    msg->type = L;

    // 获取用户输入的登录信息
    printf("请输入用户名:");
    scanf("%s", msg->name);
    printf("请输入密码:");
    scanf("%s", msg->data);

    // 保存用户名到全局消息结构体
    strcpy(msg_1->name, msg->name);

    // 发送登录请求到服务器
    send(sockfd, msg, sizeof(MSG), 0);

    // 接收服务器响应
    recv(sockfd, msg, sizeof(MSG), 0);

    // 根据响应结果显示登录成功或失败
    if (strcmp(msg->data, "OK") == 0)
    {
        printf("登录成功\n");
        // 清空数据字段
        memset(msg->data, 0, 256);
        return 1;
    }
    else
    {
        printf("登录失败:%s\n", msg->data);
        return 0;
    }
}

// 处理单词查询请求
// @param sockfd: 套接字文件描述符
// @param msg: 消息结构体，用于存储查询的单词和接收查询结果
void handle_query(int sockfd, MSG *msg)
{
    // 设置消息类型为查询单词请求
    msg->type = Q;

    // 循环查询单词，直到用户输入#结束
    while (1)
    {
        printf("请输入要查询的单词(输入#结束查询): ");
        scanf("%s", msg->data);

        // 检查是否结束查询
        if (strcmp(msg->data, "#") == 0)
        {
            getchar();  // 读取换行符
            memset(msg->data, 0, 256);  // 清空数据字段
            return;
        }

        // 发送查询请求到服务器
        send(sockfd, msg, sizeof(MSG), 0);

        // 接收服务器响应
        recv(sockfd, msg, sizeof(MSG), 0);

        // 根据响应结果显示单词解释或未找到信息
        if (strcmp(msg->data, "not found") == 0)
        {
            printf("未找到该单词\n");
        }
        else
        {
            printf("单词解释:%s\n", msg->data);
        }
    }
}

// 处理历史记录查询请求
// @param sockfd: 套接字文件描述符
// @param msg: 消息结构体，用于发送查询请求和接收历史记录
// @param msg_1: 全局消息结构体，存储登录用户信息
void handle_history(int sockfd, MSG *msg, MSG *msg_1)
{
    // 设置消息类型为查询历史记录请求
    msg->type = H;

    // 设置查询的用户名
    strcpy(msg->name, msg_1->name);

    // 发送查询请求到服务器
    send(sockfd, msg, sizeof(MSG), 0);

    // 循环接收历史记录，直到接收到空数据
    while (1)
    {
        recv(sockfd, msg, sizeof(MSG), 0);
        // 检查是否结束接收
        if (msg->data[0] == '\0')
        {
            break;
        }
        // 显示历史记录
        printf("%s\n", msg->data);
    }
    return;
}