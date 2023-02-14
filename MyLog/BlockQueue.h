#pragma once
#include<iostream>
#include<stdlib.h>
#include <pthread.h>
using namespace std;
template<class T>
class block_queue {
public:

	block_queue(int maxSize = 1000) {
		if (maxSize <= 0) exit(-1);
		this->maxSize = maxSize;
		this->back = this->front = -1;
		this->m_array = new T[maxSize];
		this->queueSize = 0;
		pthread_mutex_init(&this->m_mutex,NULL);
		pthread_cond_init(&this->m_cond,NULL);
	};
	~block_queue() { 
		if(m_array!=NULL)
		    delete []m_array;
		front = -1;
		back = -1;
		pthread_mutex_destroy(&this->m_mutex);
		pthread_cond_destroy(&this->m_cond);
	}
	int size();
	bool empty();
	bool pop(T& item);
	bool push(const T& item);
	int maxSize;//队列最大容量
private:
	int queueSize;//当前队列长度
	
	int front;//首元素
	int back;//尾元素
	T* m_array;//存储任务
	pthread_mutex_t m_mutex;
	pthread_cond_t m_cond;
	
};

template<class T>
int block_queue<T>::size() {
	int temp;
	pthread_mutex_lock(&m_mutex);
	temp = queueSize;
	pthread_mutex_unlock(&m_mutex);
	return queueSize;
}

template<class T>
bool block_queue<T>::pop(T& item) {
	pthread_mutex_lock(&m_mutex);
	while (queueSize <= 0) {
		if (pthread_cond_wait(&m_cond, &m_mutex) != 0) {
			pthread_mutex_unlock(&m_mutex);
			return false;
		}
	}
	front = (front + 1) % maxSize;
	item = m_array[front];
	queueSize--;
	pthread_mutex_unlock(&m_mutex);
	return true;
}

template<class T>
bool block_queue<T>::push(const T& item) {

	pthread_mutex_lock(&m_mutex);
	if (queueSize >= maxSize) {
		pthread_cond_broadcast(&m_cond);
		pthread_mutex_unlock(&m_mutex);
		return false;
	}
	back = (back + 1) % maxSize;
	m_array[back] = item;
	queueSize++;
	pthread_cond_broadcast(&m_cond);
	pthread_mutex_unlock(&m_mutex);
	return true;
}
template<class T>
bool block_queue<T>::empty() {
	pthread_mutex_lock(&m_mutex);
	if (queueSize <= 0) { pthread_mutex_unlock(&m_mutex); return false; }
	pthread_mutex_unlock(&m_mutex);
	return true;

}