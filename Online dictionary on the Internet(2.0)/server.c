#include"se.h"
// 服务器程序主函数
// @param argc: 参数数量
// @param argv: 参数数组，包含程序名称和服务器端口号
int main(int argc, char const *argv[])
{
    // 检查命令行参数是否正确
    if (argc < 2)
    {
        // 打印程序使用方法
        printf("Usage: %s <port>\n", argv[0]);
        return -1;
    }

    // 创建或打开SQLite数据库
    sqlite3 *ppDb = NULL;
    int ret = sqlite3_open("./student.db", &ppDb);
    if (ret != SQLITE_OK)
    {
        // 打印数据库打开错误信息
        printf("sqlite3_open = %s\n", sqlite3_errmsg(ppDb));
        return -1;
    }
    printf("打开数据库成功！\n");

    // 创建或打开账号信息表
    char sql[256];
    sprintf(sql, "create table if not exists usr (user char primary key, password char);");
    ret = sqlite3_exec(ppDb, sql, NULL, NULL, NULL);
    if (ret != SQLITE_OK)
    {
        // 打印表创建错误信息
        printf("sqlite3_open = %s\n", sqlite3_errmsg(ppDb));
        return -1;
    }
    printf("账号信息表创建成功！\n");

    // 创建或打开查询记录表
    memset(sql, 0, sizeof(sql));
    sprintf(sql, "create table if not exists record (user char, time char, word char);");
    ret = sqlite3_exec(ppDb, sql, NULL, NULL, NULL);
    if (ret != SQLITE_OK)
    {
        // 打印表创建错误信息
        printf("sqlite3_open = %s\n", sqlite3_errmsg(ppDb));
        return -1;
    }
    printf("查询记录表创建成功！\n");

    // 创建套接字文件描述符
    // AF_INET: IPv4协议
    // SOCK_STREAM: 流式套接字(TCP)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        perror("socket");  // 打印套接字创建错误信息
        return -1;
    }

    // 定义服务器和客户端地址结构体
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t addrlen = sizeof(serveraddr);

    // 设置地址族为IPv4
    serveraddr.sin_family = AF_INET;
    // 设置服务器端口号
    // htons: 将主机字节序转换为网络字节序
    // atoi: 将字符串转换为整数
    serveraddr.sin_port = htons(atoi(argv[1]));
    // 设置服务器IP地址为0.0.0.0，监听所有网络接口
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 绑定套接字到指定地址和端口
    if (0 != bind(sockfd, (const struct sockaddr *)&serveraddr, addrlen))
    {
        perror("bind");  // 打印绑定错误信息
        return -1;
    }

    // 监听套接字
    // 第二个参数为监听队列大小
    if (0 != listen(sockfd, 5))
    {
        perror("listen");  // 打印监听错误信息
        return -1;
    }

    // 定义变量
    int acceptfd;  // 客户端连接文件描述符
    ssize_t num;   // 接收/发送的数据大小

    // 创建epoll实例
    // 参数为能处理的最大文件描述符数量
    int epfd = epoll_create(2000);
    if (epfd == -1)
    {
        perror("epoll_create");  // 打印epoll创建错误信息
        return -1;
    }

    // 定义epoll事件结构体
    struct epoll_event event;
    event.events = EPOLLIN;  // 监听可读事件
    event.data.fd = sockfd;  // 设置事件关联的文件描述符

    // 将监听套接字添加到epoll
    if (-1 == epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &event))
    {
        perror("epoll_ctl");  // 打印epoll控制操作错误信息
        return -1;
    }

    // 定义变量
    int count = 0;  // 返回的事件数量
    struct epoll_event events[2000];  // 存储发生的事件

    // 主循环
    while (1)
    {
        printf("epoll阻塞等待事件的发生！\n");
        // 清空事件数组
        memset(events, 0, sizeof(events));

        // 等待事件发生
        // -1表示无限期等待
        count = epoll_wait(epfd, events, 2000, -1);
        if (count == -1)
        {
            perror("epoll_wait");  // 打印等待事件错误信息
            return -1;
        }
        printf("有 %d 个事件发生！\n", count);

        // 处理所有发生的事件
        for (int i = 0; i < count; i++)
        {
            // 检查是否为可读事件
            if (events[i].events & EPOLLIN)
            {
                // 检查是否为监听套接字的事件（新客户端连接）
                if (events[i].data.fd == sockfd)
                {
                    // 接受客户端连接
                    acceptfd = accept(sockfd, (struct sockaddr *)&clientaddr, &addrlen);
                    if (acceptfd == -1)
                    {
                        perror("accept");  // 打印接受连接错误信息
                        continue;
                    }

                    // 将新客户端套接字添加到epoll
                    struct epoll_event client_event;
                    client_event.events = EPOLLIN;  // 监听可读事件
                    client_event.data.fd = acceptfd;  // 设置事件关联的文件描述符
                    if (epoll_ctl(epfd, EPOLL_CTL_ADD, acceptfd, &client_event) == -1)
                    {
                        perror("epoll_ctl add client");  // 打印添加客户端错误信息
                        close(acceptfd);
                        continue;
                    }
                    // 打印客户端连接信息
                    printf("[%s][%d]客户端连接成功！\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
                }
                else  // 客户端发送数据
                {
                    // 获取客户端文件描述符
                    acceptfd = events[i].data.fd;
                    MSG msg;  // 定义消息结构体

                    // 接收客户端消息
                    num = recv(acceptfd, &msg, sizeof(msg), 0);
                    if (num == -1)
                    {
                        perror("recv");  // 打印接收数据错误信息
                        // 从epoll中移除客户端
                        epoll_ctl(epfd, EPOLL_CTL_DEL, acceptfd, NULL);
                        close(acceptfd);
                    }
                    else if (num == 0)
                    {
                        // 客户端关闭连接
                        printf("客户端[%d]已下线！\n", acceptfd);
                        // 从epoll中移除客户端
                        epoll_ctl(epfd, EPOLL_CTL_DEL, acceptfd, NULL);
                        close(acceptfd);
                    }
                    else
                    {
                        // 根据消息类型处理
                        switch (msg.type)
                        {
                            case R:  // 注册请求
                                handle_register(ppDb, acceptfd, &msg);
                                break;
                            case L:  // 登录请求
                                handle_login(ppDb, acceptfd, &msg);
                                break;
                            case Q:  // 查询单词请求
                                handle_query(ppDb, acceptfd, &msg);
                                break;
                            case H:  // 查询历史记录请求
                                check_history(ppDb, acceptfd, &msg);
                                break;
                        }
                    }
                }
            }
        }
    }

    // 关闭数据库
    sqlite3_close(ppDb);
    // 关闭套接字
    close(sockfd);
    return 0;
}
