#pragma once
#include"BlockQueue.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<strings.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
class Log {

public:
	static Log* getInstance() {
		static Log log;
		return &log;
	}
	static void* thread_async_write_log(void* arg) {
		Log::getInstance()->async_write_log();
	}
	bool init(const char* filename, int lineSize, int close_log, int maxQueueSize, int bufSize);
	void write_log(int level, const char* format, ...);
	void flush(void);
private:
	Log();
	virtual ~Log();
	
	void* async_write_log() {
		string item;
		while (m_log_queue->pop(item)) {
			pthread_mutex_lock(&mutex);
			fputs(item.c_str(), m_fp);
			pthread_mutex_unlock(&mutex);
		}

	}
	
	
	
	block_queue<string>* m_log_queue;//阻塞队列
	FILE* m_fp;         //打开log的文件指针
	char dir_name[128];//存放日志的目录
	char log_name[128];//日志文件名
	int  maxLineSize;//日志文件最大行数
	int bufSize;//日志缓冲区大小
	char* buf;//日志缓冲区
	bool m_is_async; //是否同步标志位
	pthread_mutex_t mutex;
	long long m_count;  //日志行数记录
	int m_close_log; //关闭日志
	int m_today;  //因为按天分类,记录当前时间是那一天
};


#define LOG_DEBUG(format, ...) if(0 == m_close_log) {Log::getInstance()->write_log(0, format, ##__VA_ARGS__); Log::getInstance()->flush();}
#define LOG_INFO(format, ...) if(0 == m_close_log) {Log::getInstance()->write_log(1, format, ##__VA_ARGS__); Log::getInstance()->flush();}
#define LOG_WARN(format, ...) if(0 == m_close_log) {Log::getInstance()->write_log(2, format, ##__VA_ARGS__); Log::getInstance()->flush();}
#define LOG_ERROR(format, ...) if(0 == m_close_log) {Log::getInstance()->write_log(3, format, ##__VA_ARGS__); Log::getInstance()->flush();}