#include"se.h"
// 处理用户注册请求
// @param db: 数据库句柄
// @param acceptfd: 客户端连接文件描述符
// @param msg: 消息结构体，包含用户名和密码
void handle_register(sqlite3 *db, int acceptfd, MSG *msg)
{
    char sql[MAX_SIZE];
    // 构建SQL插入语句
    sprintf(sql, "insert into usr (user,password) values ('%s','%s')", msg->name, msg->data);
    char *errmsg;
    // 执行SQL语句
    int ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    // 判断执行结果
    if (ret == SQLITE_OK)
    {
        strcpy(msg->data, "OK");  // 注册成功
    }
    else
    {
        strcpy(msg->data, "用户名已存在");  // 注册失败
    }
    // 发送响应给客户端
    send(acceptfd, msg, sizeof(MSG), 0);
    // 清空数据字段
    memset(msg->data, 0, 256);
}

// 处理用户登录请求
// @param db: 数据库句柄
// @param acceptfd: 客户端连接文件描述符
// @param msg: 消息结构体，包含用户名和密码
void handle_login(sqlite3 *db, int acceptfd, MSG *msg)
{
    char sql[MAX_SIZE];
    // 构建SQL查询语句
    sprintf(sql, "select * from usr where user ='%s'and password ='%s'", msg->name, msg->data);
    char **result;  // 存储查询结果
    int nrow, ncol;  // 行数和列数
    char *errmsg;
    // 执行SQL查询
    int ret = sqlite3_get_table(db, sql, &result, &nrow, &ncol, &errmsg);
    // 判断查询结果
    if (ret == SQLITE_OK && nrow == 1)
    {
        strcpy(msg->data, "OK");  // 登录成功
    }
    else
    {
        strcpy(msg->data, "用户名或者密码有误");  // 登录失败
    }
    // 发送响应给客户端
    send(acceptfd, msg, sizeof(MSG), 0);
    // 清空数据字段
    memset(msg->data, 0, 256);
}

// 查找单词定义
// @param acceptfd: 客户端连接文件描述符
// @param msg: 消息结构体，包含要查询的单词
// @return: 找到单词返回1，否则返回0
int handle_searchword(int acceptfd, MSG *msg)
{
    // 使用绝对路径打开字典文件
    FILE *fp = fopen("/home/linux/xiangmu/dict.txt", "r");
    if (fp == NULL)
    {
        strcpy(msg->data, "文件打开失败");
        send(acceptfd, msg, sizeof(MSG), 0);
        return 0;
    }

    int len = strlen(msg->data);  // 获取单词长度
    char line[300] = {0};  // 存储读取的行
    fseek(fp, 0, SEEK_SET);  // 将文件指针移动到开头

    // 逐行读取文件
    while (fgets(line, 300, fp) != NULL)
    {
        // 确保匹配的是完整单词（后面紧跟空格或换行符）
        if (strncmp(line, msg->data, len) == 0 && (line[len] == ' ' || line[len] == '\n'))
        {
            strcpy(msg->data, line);  // 复制找到的单词及解释
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;  // 未找到单词
}

/*
// 查找单词定义的另一种实现（已注释）
int do_searchword(int acceptfd, MSG *msg)
{
    FILE *fp;
    char temp[300];
    char *p;
    int len, result;

    // 得到要查找单词的长度
    len = strlen(msg->data);

    // 打开文档
    if ((fp = fopen("dict.txt", "r")) == NULL)
    {
        strcpy(msg->data, "dict can not open");
        send(acceptfd, msg, sizeof(MSG), 0);
    }

    // fgets每次只可以读一行，规定文档里面最后一行有300个字节
    while (fgets(temp, 300, fp) != NULL)
    {
        result = strncmp(msg->data, temp, len);

        // 输入的单词必须保证一样，并且temp的下一个字符是空格
        if (result == 0 && temp[len] == ' ')
        {
            // 指针指向单词后面的位置
            p = temp + len;

            // 从第一个有数据的位置开始读，排除单词与解释之间的空格
            while (*p == ' ')
            {
                p++;
            }

            strcpy(msg->data, p);

            fclose(fp);

            return 1;
        }
    }

    // 如果没找到
    fclose(fp);
    return 0;
}
*/

// 处理单词查询请求并记录历史
// @param db: 数据库句柄
// @param acceptfd: 客户端连接文件描述符
// @param msg: 消息结构体，包含要查询的单词
void handle_query(sqlite3 *db, int acceptfd, MSG *msg)
{
    char sqlstr[256], *errmsg;
    int found = 0;
    char date[128], word[128];

    strcpy(word, msg->data);  // 保存查询的单词

    // 查询单词，成功返回1
    found = handle_searchword(acceptfd, msg);

    if (found)
    {
        // 获取当前时间
        time_t now;
        struct tm *tm_info;
        time(&now);
        tm_info = localtime(&now);
        char time_str[50];
        strftime(time_str, 50, "%Y-%m-%d %H:%M:%S", tm_info);

        // 构建SQL插入语句，记录查询历史
        sprintf(sqlstr, "insert into record values('%s', '%s', '%s')", msg->name, time_str, word);
        // 执行SQL语句
        if (sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg) != SQLITE_OK)
        {
            printf("error : %s\n", errmsg);
        }
    }
    else
    {
        strcpy(msg->data, "not found");  // 未找到单词
    }

    // 发送响应给客户端
    send(acceptfd, msg, sizeof(MSG), 0);
    return;
}

/*
// 单词查询的另一种实现（已注释）
if (found)
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
}
*/

// 查询历史记录
// @param db: 数据库句柄
// @param acceptfd: 客户端连接文件描述符
// @param msg: 消息结构体，包含用户名
void check_history(sqlite3 *db, int acceptfd, MSG *msg)
{
    char sql[256];
    // 构建SQL查询语句
    sprintf(sql, "select * from record where user ='%s'", msg->name);
    char *errmsg;

    // 执行SQL查询，使用回调函数处理结果
    if (sqlite3_exec(db, sql, history_callback, (void *)&acceptfd, &errmsg) != SQLITE_OK)
    {
        printf("error : %s\n", errmsg);
        sqlite3_free(errmsg);
    }

    // 发送结束标志给客户端
    msg->data[0] = '\0';
    send(acceptfd, msg, sizeof(MSG), 0);
    return;
}

// 查询历史记录的回调函数
// @param arg: 用户数据（这里是客户端连接文件描述符）
// @param f_num: 字段数量
// @param f_value: 字段值数组
// @param f_name: 字段名数组
// @return: 0表示成功，非0表示失败
int history_callback(void *arg, int f_num, char **f_value, char **f_name)
{
    int acceptfd = *((int *)arg);  // 获取客户端文件描述符
    MSG msg;
    memset(&msg, 0, sizeof(MSG));

    // 构建历史记录信息
    sprintf(msg.data, "时间: %s, 单词: %s", f_value[1], f_value[2]);

    // 发送历史记录给客户端
    send(acceptfd, &msg, sizeof(MSG), 0);
    return 0;
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
