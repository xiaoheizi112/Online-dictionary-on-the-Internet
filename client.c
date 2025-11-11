/**
 * @file client.c
 * @brief 在线词典客户端主程序
 * 
 * 客户端程序入口，负责建立与服务器的连接，
 * 显示用户界面，并根据用户选择调用相应的功能模块。
 */
#include "cl.h"

/**
 * @brief 客户端主函数
 * 
 * 建立与服务器的TCP连接，处理用户交互，调用相应的功能函数
 * 
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组，argv[1]为服务器端口号
 * @return int 程序退出状态码
 */
int main(int argc, char const *argv[])
{
    MSG msg, msg_1; // 声明消息结构体变量
    
    // 创建TCP套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1)
    {
        perror("socket"); // 输出错误信息
        return -1;
    }
    
    // 设置服务器地址信息
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);
    server_addr.sin_family = AF_INET; // IPv4地址族
    
    // 转换端口号为网络字节序（大端序）
    server_addr.sin_port = htons(atoi(argv[1])); // 服务器端口号（从命令行参数获取）
    
    // 将点分十进制IP地址转换为网络字节序
    server_addr.sin_addr.s_addr = inet_addr("192.168.153.152"); // 服务器IP地址
    
    // 向服务器发起连接请求
    if(-1 == connect(sockfd, (const struct sockaddr *)&server_addr, addrlen))
    {
        perror("connect"); // 输出连接错误信息
        return -1;
    }
    
    // 变量定义
    ssize_t size = 0; // 用于存储接收的数据大小
    int choice;       // 用户选择的菜单选项
    char judge = 'y'; // 用于判断是否继续注册
    
    // 主循环 - 显示主菜单并处理用户选择
    while(1)
    {
        menu1(); // 显示主菜单（注册、登录、退出）
        printf("请输入选项：\n");
        scanf("%d", &choice); // 读取用户选择
        
        switch(choice)
        {        
        case REGISTER: // 注册选项
again: // 注册循环标签
            handle_Register(sockfd, &msg); // 调用注册函数
            
            // 询问用户是否继续注册
            printf("是否继续注册？Y/N\n");
            getchar(); // 清除输入缓冲区中的换行符
            judge = getchar(); // 读取用户选择
            
            if(judge == 'Y' || judge == 'y')
            {
                goto again; // 继续注册
            }
            break;
            
        case LOGIN: // 登录选项
            // 调用登录函数，如果登录成功则进入子菜单
            if(handle_login(sockfd, &msg, &msg_1))
            {
                // 登录后循环 - 显示功能菜单
                while(1)
                {
                    menu2(); // 显示功能菜单（查询单词、历史记录、退出）
                    printf("请输入选项：\n");
                    scanf("%d", &choice); // 读取用户选择
                    
                    switch(choice)
                    {
                    case WORD: // 查询单词
                        handle_query(sockfd, &msg);
                        break;
                        
                    case HISTORY: // 查询历史记录
                        handle_history(sockfd, &msg, &msg_1);
                        break;
                        
                    case QUIT2: // 退出程序
                        close(sockfd); // 关闭套接字
                        exit(0);
                        break;
                        
                    default: // 无效选择
                        printf("无效选择，请重新输入！\n");
                    }
                }
            } 
            // 注意：这里缺少break语句，会继续执行下一个case
            
        case QUIT1: // 退出程序
            close(sockfd); // 关闭套接字
            exit(0);
            break;
            
        default: // 无效选择
            printf("无效选择，请重新输入！\n");
        }
    }
    
    // 注意：由于上面有exit(0)，这里的代码实际上不会执行
    close(sockfd);
    return 0;
}
