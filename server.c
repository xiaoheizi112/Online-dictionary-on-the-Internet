#include"se.h"
int main(int argc, char const *argv[])
{
    //创建或打开一个数据库
    sqlite3 *ppDb = NULL;
    int ret = sqlite3_open("./student.db",&ppDb);
    if(ret != SQLITE_OK)
    {
        printf("sqlite3_open = %s\n",sqlite3_errmsg(ppDb));
        return -1;
    }
    printf("打开数据库成功！\n");
    //创建或打开表1(stu:账号信息表（至少包含 用户名和密码）)
    char sql[256];
    sprintf(sql,"create table if not exists usr (user char primary key,password char);");
    ret = sqlite3_exec(ppDb,sql,NULL,NULL,NULL);
    if(ret != SQLITE_OK)
    {
        printf("sqlite3_open = %s\n",sqlite3_errmsg(ppDb));
        return -1;
    }
    printf("账号信息表创建成功！\n");
    //创建或打开表2(usr:查询记录表（至少包含 用户、时间、查询的单词）)
    memset(sql,0,sizeof(sql));
    sprintf(sql,"create table if not exists record (user char,time char,word char);");
    ret = sqlite3_exec(ppDb,sql,NULL,NULL,NULL);
    if(ret != SQLITE_OK)
    {
        printf("sqlite3_open = %s\n",sqlite3_errmsg(ppDb));
        return -1;
    }
    printf("查询记录表创建成功！\n");
    //创建套接字
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd == -1)
    {
        perror("socket");
        return -1;
    }
    //绑定套接字
    struct sockaddr_in serveraddr,clientaddr;
    socklen_t addrlen = sizeof(serveraddr);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[1]));
    serveraddr.sin_addr.s_addr = inet_addr("192.168.153.152");
    if(0 != bind(sockfd,(const struct sockaddr *)&serveraddr,addrlen))
    {
        perror("bind");
        return -1;
    }
    //监听套接字
    if(0 != listen(sockfd,5))
    {
        perror("listen");
        return -1;
    }
    //接收客户端发起链接
    int acceptfd;
    ssize_t num;
   //创建一个epoll实例
    int epfd = epoll_create(2000);
    if(epfd == -1)
    {
        perror("epoll_create");
        return -1;
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd =sockfd;
    //将sockfd注册至epoll中
    if(-1 == epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&event))
    {
        perror("epoll_ctl");
        return -1;
    }
    int count =0; //用于返回事件发生的个数
    struct epoll_event events[2000]; //同时处理事件的个数
    while(1)
    {
        printf("epoll阻塞等待事件的发生！\n");
        memset(events,0,sizeof(events));
        count = epoll_wait(epfd,events,2000,-1);
        if(count == -1)
        {
            perror("epoll_wait");
            return -1;
        }
        printf("有 %d 个事件发生！\n",count);
        for (int i = 0; i < count; i++)
        {
            if(events[i].events & EPOLLIN)
            {
                if(events[i].data.fd == sockfd) //有客户端发起连接
                {
                    acceptfd = accept(sockfd,(struct sockaddr *)&clientaddr,&addrlen);
                    if(acceptfd == -1)
                    {
                        perror("accept");
                        return -1;
                    }
                    printf("[%s][%d]客户端连接成功！\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
                    event.events = EPOLLIN;
                    event.data.fd =acceptfd;
                    //将acceptfd注册至epoll中
                    if(-1 == epoll_ctl(epfd,EPOLL_CTL_ADD,acceptfd,&event))
                    {
                        perror("epoll_ctl");
                        return -1;
                    }
                }
                else  //有客户端发消息
                {      
                    while(1)
                    {
                        MSG msg;
                        if(-1 == (num = recv(acceptfd,&msg,sizeof(msg),0)))//接收客户端发送信息
                        {
                            perror("recv1");
                            break;
                        }
                        if(num == 0)
                        {
                            printf("客户端[%d]已下线！\n",acceptfd);
                            close(acceptfd);
                            continue;
                            //break;
                        }
                        switch(msg.type)
                        {
                            case R:
                                handle_register(ppDb,acceptfd,&msg);
                                break;
                            case L:
                                handle_login(ppDb,acceptfd,&msg);
                                break;
                            case Q:
                                handle_query(ppDb,acceptfd,&msg);
                                break;
                            case H:
                                check_history(ppDb,acceptfd,&msg);
                                break;
                        }                    
                    }
                }
            }
        }
    }
    close(acceptfd);
    sqlite3_close(ppDb);
    close(sockfd);
    return 0;
}
