#include"se.h"
/**
 * @brief 处理用户注册请求
 * 
 * 将新用户的用户名和密码插入到数据库中，并返回注册结果
 * 
 * @param db SQLite数据库连接指针
 * @param acceptfd 客户端连接的套接字描述符
 * @param msg 消息结构体，包含用户名和密码信息
 */
void handle_register(sqlite3 *db, int acceptfd, MSG *msg)
{
    char sql[MAX_SIZE]; // SQL语句缓冲区
    
    // 构建插入用户信息的SQL语句
    sprintf(sql, "insert into usr (user, password) values ('%s', '%s')", msg->name, msg->data);
    char *errmsg; // 存储错误信息
    
    // 执行SQL语句
    int ret = sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    
    if(ret == SQLITE_OK) // 注册成功
    {
        strcpy(msg->data, "OK"); // 设置成功标识
    } 
    else // 注册失败，通常是用户名已存在
    {
        strcpy(msg->data, "用户名已存在"); // 设置失败原因
    }
    
    // 发送结果给客户端
    send(acceptfd, msg, sizeof(MSG), 0);
    
    // 清空消息数据缓冲区
    memset(msg->data, 0, sizeof(msg->data)); // 修正：使用0而非256作为填充值
}
/**
 * @brief 处理用户登录请求
 * 
 * 验证用户提供的用户名和密码是否匹配数据库中的记录
 * 
 * @param db SQLite数据库连接指针
 * @param acceptfd 客户端连接的套接字描述符
 * @param msg 消息结构体，包含用户名和密码信息
 */
void handle_login(sqlite3 *db, int acceptfd, MSG *msg)
{
    char sql[MAX_SIZE]; // SQL语句缓冲区
    
    // 构建查询用户信息的SQL语句
    sprintf(sql, "select * from usr where user ='%s' and password ='%s'", msg->name, msg->data);
    char **result; // 存储查询结果
    int nrow, ncol; // 结果的行数和列数
    char *errmsg; // 存储错误信息
    
    // 执行SQL查询
    int ret = sqlite3_get_table(db, sql, &result, &nrow, &ncol, &errmsg);
    
    if(ret == SQLITE_OK && nrow == 1) // 查询成功且找到一条记录
    {
        strcpy(msg->data, "OK"); // 登录成功
    } 
    else // 查询失败或未找到匹配记录
    {
        strcpy(msg->data, "用户名或者密码有误"); // 登录失败
    }
    
    // 发送结果给客户端
    send(acceptfd, msg, sizeof(MSG), 0);
    
    // 清空消息数据缓冲区
    memset(msg->data, 0, sizeof(msg->data)); // 修正：使用0而非256作为填充值
}
/**
 * @brief 在词典文件中查找单词
 * 
 * 在dict.txt文件中查找指定单词并返回其释义
 * 
 * @param acceptfd 客户端连接的套接字描述符
 * @param msg 消息结构体，包含要查询的单词
 * @return int 找到返回1，未找到或出错返回0
 */
int handle_searchword(int acceptfd, MSG *msg)
{
    // 注意：这里有一个未使用的变量和无效的字符串比较操作
    char abc[256];
    strcmp(abc, msg->data); // 这个操作没有效果，因为结果没有使用
    
    // 打开词典文件
    FILE *fp = fopen("./dict.txt", "r"); // 只读打开文件
    if(fp == NULL) // 文件打开失败
    {
        strcpy(msg->data, "文件打开失败");
        send(acceptfd, msg, sizeof(MSG), 0);
        return 0;
    }
    
    // 获取单词长度
    int len = strlen(msg->data);
    char line[300] = {0}; // 用于存储文件中的每一行
    
    // 将文件指针移动到文件开头
    fseek(fp, 0, SEEK_SET);
    
    // 逐行读取文件并查找匹配的单词
    while(fgets(line, 300, fp) != NULL)
    {
        // 使用strncmp进行前缀匹配
        if(strncmp(line, msg->data, len) == 0)
        {
            // 找到匹配项，将整行内容拷贝到消息数据中
            strcpy(msg->data, line);
            fclose(fp); // 关闭文件
            return 1; // 找到单词
        }
    }
    
    // 未找到匹配项
    fclose(fp); // 关闭文件
    return 0; // 未找到单词
}
/* 
 * @brief 注释掉的备用单词查询实现 - 更精确的单词匹配方法
 * 
 * 这个函数实现了一个更精确的单词匹配算法，确保匹配的是完整单词而非前缀
 * 查找规则：不仅要求前缀匹配，还要求匹配后紧跟空格，然后只返回单词的释义部分
 * 
 * @param acceptfd 客户端连接的套接字描述符
 * @param msg 消息结构体，包含要查询的单词
 * @return int 找到返回1，未找到返回0
 */
/* int do_searchword(int acceptfd, MSG *msg)
{
	FILE *fp;
	char temp[300];
	char *p;
	int len, result;
	
	// 得到要查找单词的长度
	len = strlen(msg->data);

	// 打开词典文件
	if((fp = fopen("dict.txt", "r")) == NULL)
	{
		strcpy(msg->data, "dict can not open");
		send(acceptfd, msg, sizeof(MSG), 0);
	}

	// 逐行读取文件
	while(fgets(temp, 300, fp) != NULL)
	{
		// 比较单词前缀
		result = strncmp(msg->data, temp, len);
		
		// 更精确的匹配：确保单词完全相同，并且后面紧跟空格
		if(result == 0 && temp[len] == ' ')
		{
			// 指针指向单词后面的位置
			p = temp + len;

			// 跳过单词与解释之间的空格
			while(*p == ' ')
			{
				p++;
			}

			// 只拷贝释义部分，不包含原始单词
			strcpy(msg->data, p);
			fclose(fp);
			return 1;
		}
	}

	// 未找到单词
	fclose(fp);
	return 0;
} */
/**
 * @brief 处理单词查询请求
 * 
 * 查找单词并将查询记录保存到数据库中
 * 
 * @param db SQLite数据库连接指针
 * @param acceptfd 客户端连接的套接字描述符
 * @param msg 消息结构体，包含要查询的单词
 */
void handle_query(sqlite3 *db, int acceptfd, MSG *msg)
{
    char sqlstr[128], *errmsg; // SQL语句和错误信息
	int found = 0; // 查找结果标志
	char date[128], word[128]; // 临时变量
	
	// 保存原始单词（因为msg->data在查询后会被修改）
	strcpy(word, msg->data);
	
	// 查询单词
	found = handle_searchword(acceptfd, msg);

	if(found) // 找到单词
	{
		// 获取当前时间
		time_t now;
        struct tm *tm_info;
        time(&now); // 获取当前时间戳
        tm_info = localtime(&now); // 转换为本地时间
        char time_str[50];
        strftime(time_str, 50, "%Y-%m-%d %H:%M:%S", tm_info); // 格式化时间
		
        // 构建插入历史记录的SQL语句
		sprintf(sqlstr, "insert into record values('%s', '%s', '%s')", msg->name, time_str, word);
		
		// 将查询记录保存到数据库
		if(sqlite3_exec(db, sqlstr, NULL, NULL, &errmsg) != SQLITE_OK)
		{
			printf("error : %s\n", errmsg);
			// 注意：这里应该释放errmsg，但原代码没有
		}
	}
	else // 未找到单词
	{
		strcpy(msg->data, "not found"); // 设置未找到信息
	}
	
	// 发送查询结果给客户端
	send(acceptfd, msg, sizeof(MSG), 0);
	return;
}
/* 
 * 注释掉的旧查询实现版本
 * 注意：此版本有几个问题：
 * 1. memset参数错误（使用256作为填充值，应该是0）
 * 2. 无效的strcmp调用（结果未使用）
 * 3. 没有检查SQL执行结果
 * 4. 错误地使用msg->data作为单词存储（此时msg->data已被修改为单词释义）
 */
/* if (found) 
{        
    memset(msg->data, 0, sizeof(msg->data)); // 修正：应使用0而非256作为填充值
    strcmp(msg->data, abc); // 无效操作，结果未使用
    char sql[MAX_SIZE];
    time_t now;
    struct tm *tm_info;
    time(&now);
    tm_info = localtime(&now);
    char time_str[50];
    strftime(time_str, 50, "%Y-%m-%d %H:%M:%S", tm_info);
    // 错误：使用msg->data作为单词，此时它已经包含单词释义
    sprintf(sql, "insert into record (user, time, word) values ('%s','%s','%s')", msg->name, time_str, msg->data);
    char *errmsg;
    sqlite3_exec(db, sql, NULL, NULL, &errmsg);
    // 缺少错误处理和内存释放
}
memset(msg->data, 0, sizeof(msg->data)); // 修正：应使用0而非256作为填充值
} */
/**
 * @brief 处理查询历史记录请求
 * 
 * 从数据库中获取指定用户的查询历史记录
 * 
 * @param db SQLite数据库连接指针
 * @param acceptfd 客户端连接的套接字描述符
 * @param msg 消息结构体，包含用户名
 */
void check_history(sqlite3 *db, int acceptfd, MSG *msg)
{
    char sql[256]; // SQL语句缓冲区
    
    // 构建查询历史记录的SQL语句
    sprintf(sql, "select * from record where user ='%s'", msg->name);
    char *errmsg; // 存储错误信息
    
    // 调用sqlite3_exec执行查询，使用回调函数处理结果
	if (sqlite3_exec(db, sql, history_callback, (void *)&acceptfd, &errmsg) != SQLITE_OK)
	{
		printf("error : %s\n", errmsg);
		sqlite3_free(errmsg); // 释放错误信息内存
	}
	
	// 发送结束标记（空消息）
	msg->data[0] = '\0';
	send(acceptfd, msg, sizeof(MSG), 0);
	return;
}
    /* 
     * 注释掉的旧历史记录查询实现
     * 使用sqlite3_get_table代替回调函数获取结果
     * 注意：此版本也存在memset参数错误问题
     */
    /* int ret = sqlite3_get_table(db, sql, &result, &nrow, &ncol, &errmsg);
    int k = 1;
    int num;
    char buf[32];
    memset(msg->data, 0, sizeof(msg->data)); // 修正：应使用0而非256作为填充值
    if(ret == SQLITE_OK)
    {
        // 遍历所有记录和字段
        for(int i=0; i<nrow; i++)
        {
            for(int j=0; j<ncol; j++)
            {
                // 将所有字段值连接到消息中
                strcat(msg->data, result[k]);
                printf("msg->data = %s\n", msg->data);
                k++;
            }
            // 发送一行记录
            send(acceptfd, msg, sizeof(MSG), 0);
        }
    }
    else
    {
        strcpy(msg->data, "查询历史记录失败");
        send(acceptfd, msg, sizeof(MSG), 0);
        memset(msg->data, 0, sizeof(msg->data)); // 修正：应使用0而非256作为填充值
    }
    return; */
/**
 * @brief SQLite查询回调函数 - 处理历史记录查询结果
 * 
 * 当sqlite3_exec执行查询时，每找到一条记录就会调用此函数
 * 
 * @param arg 用户自定义参数，这里是客户端套接字描述符
 * @param f_num 结果字段数量
 * @param f_value 结果值数组
 * @param f_name 结果字段名数组
 * @return int 0表示继续处理，非0表示停止处理
 */
int history_callback(void *arg, int f_num, char **f_value, char **f_name)
{
	int acceptfd; // 客户端套接字描述符
	MSG msg; // 消息结构体
	
	// 从参数中提取套接字描述符
	acceptfd = *(int *)arg;
	
	// 构造历史记录消息：时间 + 单词
	// f_value[0]是用户名，f_value[1]是时间，f_value[2]是单词
	sprintf(msg.data, "%s : %s", f_value[1], f_value[2]);
	
	// 发送历史记录到客户端
	send(acceptfd, &msg, sizeof(MSG), 0);
	return 0; // 继续处理下一条记录
}
