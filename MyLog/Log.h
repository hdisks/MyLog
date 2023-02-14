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
	
	
	
	block_queue<string>* m_log_queue;//��������
	FILE* m_fp;         //��log���ļ�ָ��
	char dir_name[128];//�����־��Ŀ¼
	char log_name[128];//��־�ļ���
	int  maxLineSize;//��־�ļ��������
	int bufSize;//��־��������С
	char* buf;//��־������
	bool m_is_async; //�Ƿ�ͬ����־λ
	pthread_mutex_t mutex;
	long long m_count;  //��־������¼
	int m_close_log; //�ر���־
	int m_today;  //��Ϊ�������,��¼��ǰʱ������һ��
};


#define LOG_DEBUG(format, ...) if(0 == m_close_log) {Log::getInstance()->write_log(0, format, ##__VA_ARGS__); Log::getInstance()->flush();}
#define LOG_INFO(format, ...) if(0 == m_close_log) {Log::getInstance()->write_log(1, format, ##__VA_ARGS__); Log::getInstance()->flush();}
#define LOG_WARN(format, ...) if(0 == m_close_log) {Log::getInstance()->write_log(2, format, ##__VA_ARGS__); Log::getInstance()->flush();}
#define LOG_ERROR(format, ...) if(0 == m_close_log) {Log::getInstance()->write_log(3, format, ##__VA_ARGS__); Log::getInstance()->flush();}