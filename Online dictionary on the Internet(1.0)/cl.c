#include"cl.h"
//菜单1
void menu1()
{
    printf("\t\t\t\t欢迎使用网络在线词典！！！\n\n\n");
    printf("\t\t\t\t1.注册  2.登录  3.退出\n\n\n");
    printf("\t\t\t\t请选择相应序号:(1/2/3)\n\n\n");
}
//菜单2
void menu2()
{
    printf("\t\t\t\t4.查询  5.历史记录  6.退出\n\n\n");
    printf("\t\t\t\t请选择相应序号:(4/5/6)\n\n\n");
}
//注册
void handle_Register(int sockfd,MSG *msg)
{
    msg->type = R;
    printf("请输入用户信息:\n");
    printf("请输入用户名:");
    scanf("%s", msg->name);
    printf("请输入密码:");
    scanf("%s",msg->data);
    send(sockfd,msg,sizeof(MSG),0);
    recv(sockfd,msg,sizeof(MSG),0);
    if (strcmp(msg->data,"OK") == 0) 
    {
        printf("注册成功\n");
    } 
    else 
    {
        printf("注册失败: %s\n",msg->data);
    }
}
//登录
int handle_login(int sockfd,MSG *msg,MSG *msg_1) 
{
    msg->type = L;
    printf("请输入用户名:");
    scanf("%s",msg->name);
    printf("请输入密码:");
    scanf("%s",msg->data);
    strcmp(msg_1->name,msg->name);
    send(sockfd,msg,sizeof(MSG),0);
    recv(sockfd,msg,sizeof(MSG),0);
    if (strcmp(msg->data,"OK") == 0) 
    {
        printf("登录成功\n");
        memset(msg->data,256,0);
        return 1;
    } 
    else 
    {
        printf("登录失败:%s\n",msg->data);
        return 0;
    }
}
//查询单词
void handle_query(int sockfd,MSG *msg) 
{
    msg->type = Q;
    while (1) {
        printf("请输入要查询的单词(输入#结束查询): ");
        scanf("%s",msg->data);
        if(strcmp(msg->data,"#") == 0)
        {
            getchar();
            memset(msg->data,256,0);
            return;
        } 
        send(sockfd,msg,sizeof(MSG),0);
        recv(sockfd,msg,sizeof(MSG),0);
        if (strcmp(msg->data,"not found") == 0)
        {
            printf("未找到该单词\n");
        } 
        else 
        {
            printf("单词解释:%s\n",msg->data);
        }
    }
}
// 查询历史记录
void handle_history(int sockfd,MSG *msg,MSG *msg_1)
{
    msg->type = H;
    strcmp(msg->name,msg_1->name);
    send(sockfd,msg,sizeof(MSG), 0);
    while(1)
    {
        recv(sockfd,msg,sizeof(MSG),0);
        if(msg->data[0] == '\0')
		{
			break;
		}
        printf("%s\n", msg->data);
    }
    return;
}