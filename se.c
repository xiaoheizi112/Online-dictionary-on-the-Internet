#include"se.h"
//注册
void handle_register(sqlite3 *db,int acceptfd,MSG *msg)
{
    char sql[MAX_SIZE];
    sprintf(sql,"insert into usr (user,password) values ('%s','%s')",msg->name,msg->data);
    char *errmsg;
    int ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    if(ret == SQLITE_OK) 
    {
        strcpy(msg->data,"OK");
    } 
    else 
    {
        strcpy(msg->data,"用户名已存在");
    }
    send(acceptfd,msg,sizeof(MSG),0);
    memset(msg->data,256,0);
}
//登录
void handle_login(sqlite3 *db,int acceptfd,MSG *msg)
{
    char sql[MAX_SIZE];
    sprintf(sql,"select * from usr where user ='%s'and password ='%s'",msg->name,msg->data);
    char **result;
    int nrow, ncol;
    char *errmsg;
    int ret = sqlite3_get_table(db,sql,&result,&nrow,&ncol,&errmsg);
    if(ret == SQLITE_OK && nrow == 1) 
    {
        strcpy(msg->data,"OK");
    } 
    else
    {
        strcpy(msg->data,"用户名或者密码有误");
    }
    send(acceptfd,msg,sizeof(MSG),0);
    memset(msg->data,256,0);
}
//查找单词
int handle_searchword(int acceptfd,MSG *msg)
{
    char abc[256];
    strcmp(abc,msg->data);
    FILE *fp = fopen("./dict.txt","r");//只读打开文件
    if(fp == NULL)
    {
        strcpy(msg->data, "文件打开失败");
        send(acceptfd,msg,sizeof(MSG),0);
        return 0;
    }
    int len = strlen(msg->data);
    char line[300] = {0};
    fseek(fp,0,SEEK_SET);
    while(fgets(line,300,fp) != NULL)
    {
        //char *token = strtok(line, " ");
        //printf("%s",line);
        if(strncmp(line,msg->data,len) == 0)
        {
            strcpy(msg->data,line);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}
/*  int  do_searchword(int acceptfd, MSG *msg)
{
	FILE *fp;
	char temp[300];
	char *p;
	int len, result;
	
	//得到要查找单词的长度
	len = strlen(msg->data);

	//打开文档
	if((fp = fopen("dict.txt", "r")) == NULL)
	{
		strcpy(msg->data, "dict can not open");
		send(acceptfd, msg, sizeof(MSG), 0);
	}

	//printf("query word is %s len = %d\n", msg->data, len);
	
	//fgets每次只可以读一行，规定文档里面最后一行有300个字节
	while(fgets(temp, 300, fp) != NULL)
	{
		result = strncmp(msg->data, temp, len);
		
		//输入的单词必须保证一样，并且temp的下一个字符是空格
		if(result == 0 && temp[len] == ' ')
		{
			//指针指向单词后面的位置
			p = temp + len;

			//从第一个有数据的位置开始读，排除单词与解释之间的空格
			while(*p == ' ')
			{
				p++;
			}

			strcpy(msg->data, p);

			fclose(fp);

			return 1;
		}
	}

	//如果没找到
	fclose(fp);
	return 0;
} */
//查询
void handle_query(sqlite3 *db,int acceptfd,MSG *msg)
{
    char sqlstr[128], *errmsg;
	int found = 0;
	char date[128], word[128];
	
	strcpy(word, msg->data);
	
	//查询成功返回1
	found = handle_searchword(acceptfd, msg);

	if(found)
	{
		//时间
		time_t now;
        struct tm *tm_info;
        time(&now);
        tm_info = localtime(&now);
        char time_str[50];
        strftime(time_str, 50, "%Y-%m-%d %H:%M:%S", tm_info);
		sprintf(sqlstr, "insert into record values('%s', '%s', '%s')",msg->name,time_str,word);
		//将用户名、时间以及单词保存在历史记录表里
		if(sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg) != SQLITE_OK)
		{
			printf("error : %s\n", errmsg);
		}
	}
	else
	{
		strcpy(msg->data, "not found");
	}
	send(acceptfd,msg,sizeof(MSG),0);
	return;
}
/*     if (found) 
    {
        memset(msg->data,256,0);
        strcmp(msg->data,abc);
        char sql[MAX_SIZE];
        time_t now;
        struct tm *tm_info;
        time(&now);
        tm_info = localtime(&now);
        char time_str[50];
        strftime(time_str, 50, "%Y-%m-%d %H:%M:%S", tm_info);
        sprintf(sql, "insert into record (user, time, word) values ('%s','%s','%s')",msg->name,time_str,msg->data);
        char *errmsg;
        sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    }
    memset(msg->data,256,0);
} */
//查询历史记录
void check_history(sqlite3 *db,int acceptfd,MSG *msg)
{
    char sql[256];
    sprintf(sql,"select * from record where user ='%s'",msg->name);
    //sprintf(sql,"select * from record where name =%s",msg->name);
    char *errmsg;
    //调用函数查询历史记录
	if (sqlite3_exec(db,sql,history_callback,(void *)&acceptfd,&errmsg) != SQLITE_OK)
	{
		printf("error : %s\n", errmsg);
		sqlite3_free(errmsg);
	}
	msg->data[0] = '\0';
	send(acceptfd,msg,sizeof(MSG),0);
	return;
}
    /* int ret = sqlite3_get_table(db,sql,&result,&nrow,&ncol,&errmsg);
    int k =1;
    int num;
    char buf[32];
    memset(msg->data,256,0);
    if(ret == SQLITE_OK)
    {
        for(int i=0;i<nrow;i++)
        {
            for(int j=0;j<ncol;j++)
            {
                //strcmp(result[k],msg->data);
                //printf("%-10s",result[k]);
                strcat(msg->data,result[k]);
                //strcat(msg->data,"\n");
                printf("msg->data = %s\n",msg->data);
                k++;
            }
            printf("11111\n");
            send(acceptfd,msg,sizeof(MSG),0);
        }
        //printf("buf = %s\n",buf);
        //strcmp(msg->data,buf);
        //printf("%s\n",msg->data);
    }
    else
    {
        strcpy(msg->data,"查询历史记录失败");
        send(acceptfd,msg,sizeof(MSG),0);
        printf("22222\n");
        memset(msg->data,256,0);
    }
    return; */
//回调函数
int history_callback(void *arg, int f_num, char **f_value, char **f_name)
{
	int acceptfd;
	MSG msg;
	//传递文件描述符
	acceptfd = *(int *)arg;
	//只获取当前用户历史记录的时间以及单词
	sprintf(msg.data, "%s : %s", f_value[1], f_value[2]);
	send(acceptfd,&msg,sizeof(MSG),0);
	return 0;
}
