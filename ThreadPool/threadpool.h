// #program once 在vs平台下有用，Linux平台下可能失效
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
using namespace std;

//任务抽象基类
class Task {
 public:
  // 用户可以自定义任意任务类型，从Task继承，重写run方法。，实现自定义任务处理
  virtual void run() = 0;
};

// 线程池支持的模式
enum class PoolMode {
  MODE_FIXED,   //固定梳理线程
  MODE_CACHED,  //线程数量可动态增长
};

//线程类型
class Thread {
 public:
  // 启动线程
  void start();

 private:
};

// 线程池类型
class ThreadPool {
 public:
  ThreadPool();
  ~ThreadPool();

  void start(int initThreadSize = 4);

  void setMode(PoolMode mode);

  //设置初试的线程数量
  void setInitThreadSize(int size);

  // 设置task任务队列上限阈值
  void setTaskQueMaxThreadHold(int threshhold);

  // 给线程池提交任务
  void submitTask(std::shared_ptr<Task> sp);

  // 不希望用户对其拷贝与赋值操作
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

 private:
  std::vector<Thread*> threads_;  //线程列表
  size_t initThreadSize_;         //初试的线程数量
  int initThreadSize_;

  // 可能要执行任务的基类自动析构了，到时候就容易出错
  std::queue<std::shared_ptr<Task>> taskQue_;
  // 原子操作加减没必要直接上锁
  std::atomic_uint taskSize;
  int taskQueMaxThreadHold_;  // 任务队列上限1阈值

  std::mutex taskQueMtx_;  //保证任务队列的线程安全
  // TODO 条件变量
  std::condition_variable notFull_;   //表示任务队列不满
  std::condition_variable notEmpty_;  //表示任务队列不空

  PoolMode poolMode_;  //当前线程池的工作模式
 private:
  // 定义线程函数
  void threadFunc();
};

#endif  // !THREADPOOL_H
