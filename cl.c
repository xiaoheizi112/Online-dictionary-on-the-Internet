/**
 * @file cl.c
 * @brief 在线词典客户端功能实现
 * 
 * 包含客户端所有功能实现函数，如菜单显示、注册、登录、
 * 单词查询和历史记录查询等功能。
 */
#include "cl.h"

/**
 * @brief 显示主菜单
 * 
 * 向用户展示在线词典的主要功能选项：注册、登录和退出
 */
void menu1()
{
    printf("\t\t\t\t欢迎使用网络在线词典！！！\n\n\n");
    printf("\t\t\t\t1.注册  2.登录  3.退出\n\n\n");
    printf("\t\t\t\t请选择相应序号:(1/2/3)\n\n\n");
}

/**
 * @brief 显示功能菜单
 * 
 * 登录成功后向用户展示可用的功能选项：查询单词、查询历史记录和退出
 */
void menu2()
{
    printf("\t\t\t\t4.查询  5.历史记录  6.退出\n\n\n");
    printf("\t\t\t\t请选择相应序号:(4/5/6)\n\n\n");
}

/**
 * @brief 处理用户注册
 * 
 * 收集用户输入的注册信息，发送到服务器进行注册，并显示注册结果
 * 
 * @param sockfd 客户端套接字描述符
 * @param msg 消息结构体，用于存储和发送注册信息
 */
void handle_Register(int sockfd, MSG *msg)
{
    msg->type = R; // 设置消息类型为注册请求
    
    // 提示用户输入注册信息
    printf("请输入用户信息:\n");
    printf("请输入用户名:");
    scanf("%s", msg->name); // 读取用户名
    printf("请输入密码:");
    scanf("%s", msg->data); // 读取密码
    
    // 发送注册信息到服务器
    send(sockfd, msg, sizeof(MSG), 0);
    
    // 接收服务器返回的注册结果
    recv(sockfd, msg, sizeof(MSG), 0);
    
    // 根据返回结果显示注册状态
    if (strcmp(msg->data, "OK") == 0) 
    {
        printf("注册成功\n");
    } 
    else 
    {
        printf("注册失败: %s\n", msg->data);
    }
}

/**
 * @brief 处理用户登录
 * 
 * 收集用户输入的登录信息，发送到服务器进行验证，并返回登录结果
 * 
 * @param sockfd 客户端套接字描述符
 * @param msg 消息结构体，用于存储和发送登录信息
 * @param msg_1 全局消息结构体，用于保存当前登录用户信息
 * @return int 登录成功返回1，失败返回0
 */
int handle_login(int sockfd, MSG *msg, MSG *msg_1) 
{
    msg->type = L; // 设置消息类型为登录请求
    
    // 提示用户输入登录信息
    printf("请输入用户名:");
    scanf("%s", msg->name); // 读取用户名
    printf("请输入密码:");
    scanf("%s", msg->data); // 读取密码
    
    // 保存用户名到全局消息结构体（修正：使用strcpy而非strcmp）
    strcpy(msg_1->name, msg->name);
    
    // 发送登录信息到服务器
    send(sockfd, msg, sizeof(MSG), 0);
    
    // 接收服务器返回的登录结果
    recv(sockfd, msg, sizeof(MSG), 0);
    
    // 根据返回结果显示登录状态
    if (strcmp(msg->data, "OK") == 0) 
    {
        printf("登录成功\n");
        memset(msg->data, 0, sizeof(msg->data)); // 修正：使用0而非256作为填充值
        return 1; // 登录成功
    } 
    else 
    {
        printf("登录失败:%s\n", msg->data);
        return 0; // 登录失败
    }
}

/**
 * @brief 处理单词查询
 * 
 * 收集用户输入的单词，发送到服务器进行查询，并显示查询结果
 * 
 * @param sockfd 客户端套接字描述符
 * @param msg 消息结构体，用于存储和发送查询信息
 */
void handle_query(int sockfd, MSG *msg) 
{
    msg->type = Q; // 设置消息类型为查询请求
    
    // 单词查询循环
    while (1) 
    {
        printf("请输入要查询的单词(输入#结束查询): ");
        scanf("%s", msg->data); // 读取要查询的单词
        
        // 检查是否退出查询
        if(strcmp(msg->data, "#") == 0)
        {
            getchar(); // 清除输入缓冲区中的换行符
            memset(msg->data, 0, sizeof(msg->data)); // 修正：使用0而非256作为填充值
            return; // 结束查询函数
        } 
        
        // 发送查询请求到服务器
        send(sockfd, msg, sizeof(MSG), 0);
        
        // 接收查询结果
        recv(sockfd, msg, sizeof(MSG), 0);
        
        // 显示查询结果
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

/**
 * @brief 处理历史记录查询
 * 
 * 向服务器请求并接收当前用户的查询历史记录，然后显示给用户
 * 
 * @param sockfd 客户端套接字描述符
 * @param msg 消息结构体，用于发送和接收历史记录
 * @param msg_1 全局消息结构体，包含当前登录用户信息
 */
void handle_history(int sockfd, MSG *msg, MSG *msg_1)
{
    msg->type = H; // 设置消息类型为历史记录查询请求
    
    // 复制当前登录用户名（修正：使用strcpy而非strcmp）
    strcpy(msg->name, msg_1->name);
    
    // 发送历史记录查询请求到服务器
    send(sockfd, msg, sizeof(MSG), 0);
    
    // 循环接收历史记录
    while(1)
    {
        recv(sockfd, msg, sizeof(MSG), 0);
        
        // 检查是否接收完毕（服务器发送空字符串表示结束）
        if(msg->data[0] == '\0')
	{
		break; // 结束循环
	}
        
        // 显示一条历史记录
        printf("%s\n", msg->data);
    }
    
    return;
}