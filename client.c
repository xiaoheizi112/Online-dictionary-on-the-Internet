#include "cl.h"

// 客户端程序主函数
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

    // 定义消息结构体变量
    MSG msg, msg_1;
    // 初始化消息结构体为0
    memset(&msg, 0, sizeof(MSG));
    memset(&msg_1, 0, sizeof(MSG));

    // 创建套接字文件描述符
    // AF_INET: IPv4协议
    // SOCK_STREAM: 流式套接字(TCP)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 检查套接字是否创建成功
    if (sockfd == -1)
    {
        perror("socket");  // 打印错误信息
        return -1;
    }

    // 定义服务器地址结构体
    struct sockaddr_in server_addr;
    // 获取地址结构体大小
    socklen_t addrlen = sizeof(server_addr);

    // 设置地址族为IPv4
    server_addr.sin_family = AF_INET;

    // 设置服务器端口号
    // htons: 将主机字节序转换为网络字节序
    // atoi: 将字符串转换为整数
    server_addr.sin_port = htons(atoi(argv[1]));

    // 设置服务器IP地址
    server_addr.sin_addr.s_addr = inet_addr("192.168.153.152");

    // 向服务器发起连接请求
    if (-1 == connect(sockfd, (const struct sockaddr *)&server_addr, addrlen))
    {
        perror("connect");  // 打印连接错误信息
        return -1;
    }

    // 定义变量
    ssize_t size = 0;  // 用于存储接收/发送的数据大小
    int choice;        // 用于存储用户选择
    char judge = 'y';  // 用于判断是否继续注册
    char abc = 'y';    // 未使用的变量

    // 主循环
    while (1)
    {
        // 显示主菜单
        menu1();

        // 提示用户输入选项
        printf("请输入选项：\n");

        // 读取用户输入
        scanf("%d", &choice);

        // 根据用户选择执行不同操作
        switch (choice)
        {
        case REGISTER:  // 注册选项
again:  // 注册标签，用于循环注册
            // 处理注册请求
            handle_Register(sockfd, &msg);

            // 询问用户是否继续注册
            printf("是否继续注册？Y/N\n");

            // 读取换行符
            getchar();

            // 读取用户输入
            judge = getchar();

            // 如果用户选择继续注册，则跳转到again标签
            if (judge == 'Y' || judge == 'y')
            {
                goto again;
            }
            break;

        case LOGIN:  // 登录选项
            // 处理登录请求，如果登录成功返回1
            if (handle_login(sockfd, &msg, &msg_1))
            {
                // 登录成功后进入子菜单循环
                while (1)
                {
                    // 显示查询菜单
                    menu2();

                    // 提示用户输入选项
                    printf("请输入选项：\n");

                    // 读取用户输入
                    scanf("%d", &choice);

                    // 根据用户选择执行不同操作
                    switch (choice)
                    {
                    case WORD:  // 查询单词选项
                        // 处理单词查询请求
                        handle_query(sockfd, &msg);
                        break;

                    case HISTORY:  // 查询历史记录选项
                        // 处理历史记录查询请求
                        handle_history(sockfd, &msg, &msg_1);
                        break;

                    case QUIT2:  // 退出选项
                        // 关闭套接字
                        close(sockfd);
                        // 退出程序
                        exit(0);
                        break;

                    default:
                        // 无效选择提示
                        printf("无效选择，请重新输入！\n");
                    }
                }
            }
            break;

        case QUIT1:  // 退出程序选项
            // 关闭套接字
            close(sockfd);
            // 退出程序
            exit(0);
            break;

        default:
            // 无效选择提示
            printf("无效选择，请重新输入！\n");
        }
    }
}
