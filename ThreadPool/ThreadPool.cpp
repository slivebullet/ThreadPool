#include "threadpool.h"
#include <functional>

const int TASK_MAX_THRESHHOLD = 1024;

ThreadPool::ThreadPool()
    : initThreadSize_(4),
      taskSize(0),
      taskQueMaxThreadHold_(TASK_MAX_THRESHHOLD),
      poolMode_(PoolMode::MODE_CACHED) {}

ThreadPool::~ThreadPool() {}

void ThreadPool::setMode(PoolMode mode) {
  poolMode_ = mode;
}

void ThreadPool::setTaskQueMaxThreadHold(int threshold) {
  taskQueMaxThreadHold_ = threshold;
}

void ThreadPool::submitTask(std::shared_ptr<Task> sp) {

}

void ThreadPool::start(int initThreadSize) {
  // 记录初始线程个数
  initThreadSize_ = initThreadSize;

  //创建线程对象
  for (int i = 0; i < initThreadSize_; i++) {
	//创建thread对象的时候，把线程函数给thread对象
    threads_.emplace_back(new Thread(std::bind(&ThreadPool::threadFunc, this)));
  }

  // 启动所有线程
  for (int i = 0; i < initThreadSize_; i++) {
    threads_[i]->start(); //需要去执行一个线程函数
  }
}

void ThreadPool::threadFunc() {

}

///////////////////////////////// 线程方法实现
//启动线程
void Thread::start() {

}