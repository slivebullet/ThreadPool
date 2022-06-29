#include <chrono>
#include <iostream>
#include <thread>

#include "threadpool.h"

/*��Щ����ϣ����ȡ�û�����ֵ
������ 1+....+30000�ĺ�
thread1 1+....+10000
thread2 10001+.....20000
.......
main thread����ÿһ���̷߳������ռ䣬�ȴ��������귵�ؽ�����ϲ����յĽ��

*/

//class Any {
// public:
//  Any() = default;
//  ~Any() = default;
//  Any(const Any&) = delete;
//  Any& operator=(const Any&) = delete;
//  Any& operator=(Any&&) = default;
//
//  // ������캯��������Any���ͽ�����������������
//  template <typename T>
//  Any(T data) : base_(std::make_unique<Derive<T>>(data)) {}
//
//  // ��ȡ��data_����
//  template <typename T>
//  T cast_() {
//    // ����ָ��---��������ָ��
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
//    T data_;  //�������������������
//  };
//
// private:
//  // ����һ������ָ��
//  std::unique_ptr<Base> base_;
//};
//
//// ʵ��һ���ź�����
//class Semaphore {
// public:
//  Semaphore(int limit = 0) : resLimit_(limit) {}
//  ~Semaphore() = default;
//
//  // ��ȡһ���ź�����Դ
//  void wait() { std::unique_lock<std::mutex> lock(mtx_);
//    cond_.wait(lock, [&]() -> bool { return resLimit_ > 0; });
//    resLimit_--;
//  }
//
//  // ����һ���ź�����Դ
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
//// ʵ�ֽ����ύ���̳߳ص�task����ִ����ɺ�ķ���ֵ����Result
//class Result {
// public:
//
// private:
//  Any any_; // �洢����ķ���ֵ
//  Semaphore sem_; // �߳�ͨ���ź���
//};


class MyTask : public Task {
 public:
  MyTask(int begin, int end) : begin_(begin), end_(end) {}
  /*
  ����1�� ��ô���run�����ķ���ֵ�����Ա�ʾ���������
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

  // �������Result���ƣ�
  Result res = pool.submitTask(std::make_shared<MyTask>());
  int sum = res.get().cast_<int>();  // get������һ��Any���ͣ���ôת���ɾ�������

  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());
  pool.submitTask(std::make_shared<MyTask>());

  getchar();
}
