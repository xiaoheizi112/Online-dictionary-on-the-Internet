#include"se.h"

/**
 * @brief 服务器主函数 - 处理客户端连接和请求
 * 
 * 该函数实现了基于Socket和Epoll的网络服务器，提供在线词典服务。
 * 主要功能包括：初始化数据库、创建套接字、处理客户端连接、
 * 使用Epoll进行IO多路复用，处理客户端的注册、登录、查询单词和历史记录等请求。
 * 
 * @param argc 命令行参数数量
 * @param argv 命令行参数数组，argv[1]为端口号
 * @return int 程序退出状态码
 */
int main(int argc, char const *argv[])
{
    // 创建或打开SQLite数据库
    sqlite3 *ppDb = NULL; // 数据库连接指针
    int ret = sqlite3_open("./student.db", &ppDb); // 打开或创建数据库文件
    if(ret != SQLITE_OK) // 检查数据库打开是否成功
    {
        printf("数据库打开失败: %s\n", sqlite3_errmsg(ppDb));
        return -1;
    }
    printf("打开数据库成功！\n");
    
    // 创建用户表（如果不存在）
    char sql[256]; // SQL语句缓冲区
    // 创建用户账号信息表，包含用户名(主键)和密码
    //sprintf:格式化字符串输出到字符数组sql里面
    // "create table if not exists"：如果表不存在，则创建该表
    // "usr"：表名，用于存储用户账号信息
    // "user char primary key"：用户名字段，类型为字符型，主键，确保每个用户账号唯一
    // "password char"：密码字段，类型为字符型，用于存储用户密码
    sprintf(sql, "create table if not exists usr (user char primary key, password char);");
    ret = sqlite3_exec(ppDb, sql, NULL, NULL, NULL); // 执行SQL语句
    if(ret != SQLITE_OK) // 检查表创建是否成功
    {
        printf("用户表创建失败: %s\n", sqlite3_errmsg(ppDb));
        return -1;
    }
    printf("账号信息表创建成功！\n");
    
    // 创建查询记录表（如果不存在）
    memset(sql, 0, sizeof(sql)); // 清空SQL缓冲区
    // 创建查询记录表，包含用户名、查询时间和查询单词
    sprintf(sql, "create table if not exists record (user char, time char, word char);");
    ret = sqlite3_exec(ppDb, sql, NULL, NULL, NULL); // 执行SQL语句
    if(ret != SQLITE_OK) // 检查表创建是否成功
    {
        printf("查询记录表创建失败: %s\n", sqlite3_errmsg(ppDb));
        return -1;
    }
    printf("查询记录表创建成功！\n");
    
    // 创建套接字 - 使用IPv4协议，TCP连接
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) // 检查套接字创建是否成功
    {
        perror("套接字创建失败");
        return -1;
    }
    
    // 绑定套接字到指定IP和端口
    struct sockaddr_in serveraddr, clientaddr; // 服务器和客户端地址结构体
    socklen_t addrlen = sizeof(serveraddr);
    serveraddr.sin_family = AF_INET; // IPv4地址族
    serveraddr.sin_port = htons(atoi(argv[1])); // 端口号（从命令行参数获取并转换为网络字节序）
    serveraddr.sin_addr.s_addr = inet_addr("192.168.153.152"); // 服务器IP地址
    if(0 != bind(sockfd, (const struct sockaddr *)&serveraddr, addrlen)) // 检查绑定是否成功
    {
        perror("套接字绑定失败");
        return -1;
    }
    
    // 设置套接字为监听模式
    if(0 != listen(sockfd, 5)) // 检查监听设置是否成功
    {
        perror("监听设置失败");
        return -1;
    }
    
    // 变量定义
    int acceptfd; // 用于存储与客户端连接的套接字描述符
    ssize_t num; // 用于存储接收的数据大小
    
    // 创建Epoll实例，用于IO多路复用
    int epfd = epoll_create(2000); // 创建可以处理2000个事件的epoll实例
    if(epfd == -1) // 检查epoll实例创建是否成功
    {
        perror("Epoll创建失败");
        return -1;
    }

    // 配置epoll事件结构体
    struct epoll_event event; // epoll事件结构体
    event.events = EPOLLIN; // 监听读事件
    event.data.fd = sockfd; // 关联到服务器监听套接字
    
    // 将监听套接字添加到epoll中
    if(-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event)) // 检查添加是否成功
    {
        perror("Epoll控制操作失败");
        return -1;
    }
    
    // 定义用于epoll_wait的变量
    int count = 0; // 用于返回事件发生的个数
    struct epoll_event events[2000]; // 存储发生事件的数组
    
    // 主循环 - 不断处理客户端请求
    while(1)
    {
        printf("epoll阻塞等待事件的发生！\n");
        memset(events, 0, sizeof(events)); // 清空事件数组
        
        // 等待事件发生，-1表示无限等待
        count = epoll_wait(epfd, events, 2000, -1);
        if(count == -1) // 检查epoll_wait是否失败
        {
            perror("Epoll等待失败");
            return -1;
        }
        printf("有 %d 个事件发生！\n", count);
        
        // 处理所有发生的事件
        for (int i = 0; i < count; i++)
        {
            // 检查是否为读事件
            if(events[i].events & EPOLLIN)
            {
                // 如果是监听套接字的事件，表示有新的客户端连接
                if(events[i].data.fd == sockfd)
                {
                    // 接受客户端连接
                    acceptfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen);
                    if(acceptfd == -1) // 检查接受连接是否失败
                    {
                        perror("接受连接失败");
                        return -1;
                    }
                    
                    // 打印客户端连接信息
                    printf("[%s][%d]客户端连接成功！\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
                    
                    // 配置新连接的epoll事件
                    event.events = EPOLLIN; // 监听读事件
                    event.data.fd = acceptfd; // 关联到新的客户端连接
                    
                    // 将新的客户端连接添加到epoll中
                    if(-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, acceptfd, &event)) // 检查添加是否成功
                    {
                        perror("Epoll控制操作失败");
                        return -1;
                    }
                }
                else // 如果是客户端连接的事件，表示客户端发送了数据
                {      
                    // 处理客户端消息的循环
                    while(1)
                    {
                        MSG msg; // 消息结构体
                        // 接收客户端发送的信息
                        if(-1 == (num = recv(events[i].data.fd, &msg, sizeof(msg), 0)))
                        {
                            perror("接收数据失败");
                            break;
                        }
                        
                        // 检查客户端是否断开连接
                        if(num == 0)
                        {
                            printf("客户端[%d]已下线！\n", events[i].data.fd);
                            close(events[i].data.fd); // 关闭连接
                            continue;
                        }
                        
                        // 根据消息类型调用相应的处理函数
                        switch(msg.type)
                        {
                            case R: // 注册请求
                                handle_register(ppDb, events[i].data.fd, &msg);
                                break;
                            case L: // 登录请求
                                handle_login(ppDb, events[i].data.fd, &msg);
                                break;
                            case Q: // 查询单词请求
                                handle_query(ppDb, events[i].data.fd, &msg);
                                break;
                            case H: // 查询历史记录请求
                                check_history(ppDb, events[i].data.fd, &msg);
                                break;
                        }                    
                    }
                }
            }
        }
    }
    
    // 清理资源（注意：由于主循环是无限循环，这些代码实际上不会被执行）
    close(acceptfd); // 关闭客户端连接
    sqlite3_close(ppDb); // 关闭数据库连接
    close(sockfd); // 关闭服务器监听套接字
    
    return 0;
}
