#include"cl.h"

int main(int argc, char const *argv[])
{
    MSG msg,msg_1;
    //创建套接字文件
    int  sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1)
    {
        perror("socket");
        return -1;
    }
    //绑定套接字文件
    struct sockaddr_in server_addr;
    socklen_t addrlen = sizeof(server_addr);
    server_addr.sin_family = AF_INET;
    //因为在网络中，都需要网络序（大端序）而此时本地是小端序，所以端口号和ip地址都需要进行转化
    server_addr.sin_port =htons(atoi(argv[1]));//服务器端口号
    server_addr.sin_addr.s_addr = inet_addr("192.168.153.152");
    //向服务器发起链接请求
    if(-1 ==connect(sockfd, (const struct sockaddr *)&server_addr,addrlen))
    {
        perror("connet");
        return -1;
    }
    ssize_t size = 0;
    int choice;
    char judge='y';
    char abc='y';
    while(1)
    {
        menu1();
        printf("请输入选项：\n");
        scanf("%d",&choice);
        switch(choice)
        {
        case REGISTER://注册
again:
            handle_Register(sockfd,&msg);
            printf("是否继续注册？Y/N\n");
            getchar();
            judge=getchar();
            if(judge == 'Y'|| judge == 'y')
            {
                goto again;
            }
            break;
        case LOGIN://登录
            if(handle_login(sockfd,&msg,&msg_1))
            {
                while(1)
                {
                    menu2();
                    printf("请输入选项：\n");
                    scanf("%d",&choice);
                    switch(choice)
                    {
                    case WORD://查询单词请求
                        handle_query(sockfd,&msg);
                        break;
                    case HISTORY://查询历史记录请求
                        handle_history(sockfd,&msg,&msg_1);
                        break;
                    case QUIT2://退出
                        close(sockfd);
                        exit(0);
                        break;
                    default:
                        printf("无效选择，请重新输入！\n");
                    }
                }
            } 
        case QUIT1://退出
            close(sockfd);
            exit(0);
            break;
        default:
            printf("无效选择，请重新输入！\n");
        }
    }
    close(sockfd);
    return 0;
}
