#include"Log.h"

Log::Log() {
	m_is_async = false;
	m_count;
}

Log::~Log() {
	if (m_log_queue != NULL) {
		delete m_log_queue;
	}
	if (m_fp != NULL) {
		fclose(m_fp);
	}
	pthread_mutex_destroy(&mutex);
}

bool Log::init(const char* filename, int lineSize, int close_log, int maxQueueSize, int bufSize){
	if (maxQueueSize > 0) {
		m_log_queue = new block_queue<string>(maxQueueSize);
		m_is_async = true;
		pthread_t pid;
		pthread_create(&pid, NULL, thread_async_write_log, NULL);

	}
	maxLineSize = lineSize;
	m_close_log = close_log;
	this->bufSize = bufSize;
	buf = new char[bufSize];
	memset(buf, '\0', bufSize);
	pthread_mutex_init(&mutex,NULL);

	/*time_t t = time(NULL);
	struct tm* sys_tm = localtime(&t);
	struct tm my_tm = *sys_tm;*/

	time_t t = time(NULL);
	struct tm *sys_tm = localtime(&t);
	struct tm my_tm = *sys_tm;
	m_today = my_tm.tm_mday;

	const char* p = strrchr(filename, '/');
	char log_full_name[256] = { 0 };
	if (p == NULL) {
		snprintf(log_full_name, 255, "%d_%02d_%02d_%s", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, filename);

	}
	else {
		strcpy(log_name, p + 1);
		//strcpy(dir_name, p + 1);
		strncpy(dir_name, filename, strlen(filename)-strlen(p)+1);
		snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s", dir_name, my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, log_name);
		//printf(log_full_name);
	}

	m_fp = fopen(log_full_name, "a");
	if (m_fp == NULL)
	{
		return false;
	}

	return true;
}

void Log::write_log(int level, const char* format, ...)
{
	struct timeval now = { 0, 0 };
	gettimeofday(&now, NULL);
	time_t t = now.tv_sec;
	struct tm* sys_tm = localtime(&t);
	struct tm my_tm = *sys_tm;

	char s[16] = { 0 };
	switch (level)
	{
	case 0:
		strcpy(s, "[debug]:");
		break;
	case 1:
		strcpy(s, "[info]:");
		break;
	case 2:
		strcpy(s, "[warn]:");
		break;
	case 3:
		strcpy(s, "[erro]:");
		break;
	default:
		strcpy(s, "[info]:");
		break;
	}

	pthread_mutex_lock(&mutex);
	m_count++;
	if (m_today != my_tm.tm_mday || m_count% maxLineSize==0) {
		fflush(m_fp);
		fclose(m_fp);
		char new_log[256] = { 0 };
		char tail[16] = { 0 };
		snprintf(tail, 16, "%d_%02d_%02d_", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday);

		if (m_today != my_tm.tm_mday) {
			snprintf(new_log, 255, "%s%s%s", dir_name, tail, log_name);
			m_today = my_tm.tm_mday;
			m_count = 0;
		}
		else {

			snprintf(new_log, 255, "%s%s%s.%lld", dir_name, tail, log_name, m_count / maxLineSize);

		}
		m_fp = fopen(new_log, "a");
	}

	pthread_mutex_unlock(&mutex);
	va_list valst;
	va_start(valst, format);

	string log_str;
	pthread_mutex_lock(&mutex);

	int n = snprintf(buf, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",
		my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday,
		my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, now.tv_usec, s);

	int m = vsnprintf(buf + n, bufSize - 1, format, valst);
	buf[n + m] = '\n';
	buf[n + m + 1] = '\0';
	log_str = buf;

	pthread_mutex_unlock(&mutex);
	int temp = m_log_queue->size();
	if (m_is_async && temp < m_log_queue->maxSize)
	{
		m_log_queue->push(log_str);
		
	}
	else
	{
		pthread_mutex_lock(&mutex);
		fputs(log_str.c_str(), m_fp);
		pthread_mutex_unlock(&mutex);
	}

	va_end(valst);
}


void Log::flush(void)
{
	pthread_mutex_lock(&mutex);
	//强制刷新写入流缓冲区
	fflush(m_fp);
	pthread_mutex_unlock(&mutex);
}
