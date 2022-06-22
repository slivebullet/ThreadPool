#include "threadpool.h"

#include <functional>
#include <iostream>
#include <thread>

const int TASK_MAX_THRESHHOLD = 1024;

ThreadPool::ThreadPool()
    : initThreadSize_(4),
      taskSize(0),
      taskQueMaxThreadHold_(TASK_MAX_THRESHHOLD),
      poolMode_(PoolMode::MODE_CACHED) {}

ThreadPool::~ThreadPool() {}

void ThreadPool::setMode(PoolMode mode) { poolMode_ = mode; }

void ThreadPool::setTaskQueMaxThreadHold(int threshold) {
  taskQueMaxThreadHold_ = threshold;
}

//给线程池提交任务	    用户调用该接口传入任务对象，生成任务
void ThreadPool::submitTask(std::shared_ptr<Task> sp) {}

void ThreadPool::start(int initThreadSize) {
  // 记录初始线程个数
  initThreadSize_ = initThreadSize;

  //创建线程对象
  for (int i = 0; i < initThreadSize_; i++) {
    //创建thread对象的时候，把线程函数给thread对象
    auto ptr =
        std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this));
    threads_.emplace_back(std::move(ptr));
  }

  // 启动所有线程
  for (int i = 0; i < initThreadSize_; i++) {
    threads_[i]->start();  //需要去执行一个线程函数
  }
}

// 定义线程函数  线程池的所有线程从任务队列里面消费任务
void ThreadPool::threadFunc() {
  std::cout << "begin threadFunc tid: ";
  std::cout << std::this_thread::get_id() << std::endl;
  std::cout << "end threadFunc tid : " << std::this_thread::get_id()
            << std::endl;
}

///////////////////////////////// 线程方法实现

// 线程构造
Thread::Thread(ThreadFunc func) : func_(func) {}

Thread::~Thread(){};

//启动线程
void Thread::start() {
  //创建一个线程来执行一个线程函数
  std::thread t(func_);

  // 设置分离线程
  t.detach();
}