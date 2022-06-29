#include <chrono>
#include <iostream>
#include <thread>

#include "threadpool.h"

/*有些场景希望获取用户返回值
举例： 1+....+30000的和
thread1 1+....+10000
thread2 10001+.....20000
.......
main thread：给每一个线程分配计算空间，等待他们算完返回结果，合并最终的结果

*/

//class Any {
// public:
//  Any() = default;
//  ~Any() = default;
//  Any(const Any&) = delete;
//  Any& operator=(const Any&) = delete;
//  Any& operator=(Any&&) = default;
//
//  // 这个构造函数可以让Any类型接收任意其它的数据
//  template <typename T>
//  Any(T data) : base_(std::make_unique<Derive<T>>(data)) {}
//
//  // 提取出data_数据
//  template <typename T>
//  T cast_() {
//    // 基类指针---》派生类指针
//    Derive<T>* pd = dynamic_cast<Derive<T>>(base_.get());
//    if (pd == nullptr) {
//      throw "type is unmatch!";
//    }
//    return pd->data_;
//  }
//
// private:
//  class Base {
//   public:
//    virtual ~Base() = default;
//  };
//
//  template <typename T>
//  class Derive : public Base {
//   public:
//    Derive(T data) : data_(data) {}
//    T data_;  //保存了任意的其他类型
//  };
//
// private:
//  // 定义一个基类指针
//  std::unique_ptr<Base> base_;
//};
//
//// 实现一个信号量类
//class Semaphore {
// public:
//  Semaphore(int limit = 0) : resLimit_(limit) {}
//  ~Semaphore() = default;
//
//  // 获取一个信号量资源
//  void wait() { std::unique_lock<std::mutex> lock(mtx_);
//    cond_.wait(lock, [&]() -> bool { return resLimit_ > 0; });
//    resLimit_--;
//  }
//
//  // 增加一个信号量资源
//  void post() {
//    std::unique_lock<std::mutex> lock(mtx_);
//    resLimit_++;
//    cond_.notify_all();
//  }
//
// private:
//  int resLimit_;
//  std::mutex mtx_;
//  std::condition_variable cond_;
//};
//
//// 实现接收提交到线程池的task任务执行完成后的返回值类型Result
//class Result {
// public:
//
// private:
//  Any any_; // 存储任务的返回值
//  Semaphore sem_; // 线程通信信号量
//};


class MyTask : public Task {
 public:
  MyTask(int begin, int end) : begin_(begin), end_(end) {}
  /*
  问题1： 怎么设计run函数的返回值，可以表示任意的类型
  */
  Any run() {
    // std::cout << "tid: " << std::this_thread::get_id() << "begin"<<std::endl;
    // std::this_thread::sleep_for(std::chrono::seconds(2));

    int sum = 0;
    for (int i = begin_; i < end_; i++) sum += i;
  }

 private:
  int begin_;
  int end_;
};

int main() {
  ThreadPool pool;
  pool.start(4);

  // 如何设置Result机制？
  Result res = pool.submitTask(std::make_shared<MyTask>());
  int sum = res.get().cast_<int>();  // get返回了一个Any类型，怎么转化成具体类型

  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());

  getchar();
}
