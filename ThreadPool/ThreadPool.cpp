#include "threadpool.h"

#include <functional>
#include <iostream>
#include <thread>

const int TASK_MAX_THRESHHOLD = 1024;

ThreadPool::ThreadPool()
    : initThreadSize_(4),
      taskSize_(0),
      taskQueMaxThreadHold_(TASK_MAX_THRESHHOLD),
      poolMode_(PoolMode::MODE_CACHED) {}

ThreadPool::~ThreadPool() {}

void ThreadPool::setMode(PoolMode mode) { poolMode_ = mode; }

void ThreadPool::setTaskQueMaxThreadHold(int threshold) {
  taskQueMaxThreadHold_ = threshold;
}

//给线程池提交任务	    用户调用该接口传入任务对象，生成任务
Result ThreadPool::submitTask(std::shared_ptr<Task> sp) {
  // 获取锁
  std::unique_lock<std::mutex> lock(taskQueMtx_);
  // 线程的通信  等待任务队列有空余
  //用户提交任务，最长不能阻塞超过1s，否则判断提交任务失败，返回
  // while (taskQue_.size() == taskQueMaxThreadHold_) {
  //  // 当前线程进入等待状态
  //  notFull_.wait(lock);
  //}

  /*
        wait： 一直等，等到条件满足再向下执行
        wait_for： 增加了一个时间参数，持续等待时间
  */
  // 两个条件：先满足后面的或者等到一秒才返回
  if (!notFull_.wait_for(lock, std::chrono::seconds(1), [&]() -> bool {
        return taskQue_.size() < taskQueMaxThreadHold_;
      })) {
    // 表示notFull_等待1s，条件仍然没有满足
    std::cerr << "task queue is full, submit task fail." << std::endl;
    return;
  }
  // 如果有空余，把任务放入任务队列中
  taskQue_.emplace(sp);
  taskSize_++;
  // 因为新放了任务，任务队列肯定不空，
  // 在notEmpty_上进行通知,赶快分配线程执行任务
  notEmpty_.notify_all();

  // 返回Result对象
}

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
  // std::cout << "begin threadFunc tid: ";
  // std::cout << std::this_thread::get_id() << std::endl;
  // std::cout << "end threadFunc tid : " << std::this_thread::get_id()
  //           << std::endl;
  std::shared_ptr<Task> task;
  for (;;) {
    {
      // 先获取锁
      std::unique_lock<std::mutex> lock(taskQueMtx_);

      std::cout << "tid: " << std::this_thread::get_id() << "尝试获取任务"
                << std::endl;

      // 等待notEmpty条件
      notEmpty_.wait(lock, [&]() -> bool { return taskQue_.size() > 0; });

      std::cout << "tid: " << std::this_thread::get_id() << "获取任务成功"
                << std::endl;

      //从任务队列中取一个任务出来
      task = taskQue_.front();
      taskQue_.pop();
      taskSize_--;

      // 如果依然有剩余任务，继续通知其他的线程执行任务
      if (taskQue_.size() > 0) {
        notEmpty_.notify_all();
      }

      // 取出一个任务进行通知,通知可以继续提交申请任务
      notFull_.notify_all();

    }  // 出了局部作用域锁自动释放掉

	// 当前线程负责执行这个任务
    if (task) {
      //task->run();  //执行任务， 把任务的返回值setVal方法给到Result
      task->exec();
    }
  }
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

//////  Task方法实现
void Task::exec() { 
  result_->setVal(run()); // 这里发生多态调用
}

/*-------------------Result方法的实现------------------------------------*/
Result::Result(std::shared_ptr<Task> task, bool isValid)
    : isValid_(isValid), task_(task) {
  task->setResult(this);
}

// 用户调用
Any Result::get() {
  if (!isValid_) {
    return "";
  }
  sem_.wait();	// task任务如果没有执行完，这里会阻塞用户的线程
  return std::move(any_);
}

// 
void Result::setVal(Any any) { 
  this->any_ = std::move(any); 
  sem_.post();
}