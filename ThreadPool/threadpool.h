// #program once ��vsƽ̨�����ã�Linuxƽ̨�¿���ʧЧ
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
using namespace std;

class Any {
 public:
  Any() = default;
  ~Any() = default;
  Any(const Any&) = delete;
  Any& operator=(const Any&) = delete;
  Any(Any&&) = default;
  Any& operator=(Any&&) = default;

  // ������캯��������Any���ͽ�����������������
  template <typename T>
  Any(T data) : base_(std::make_unique<Derive<T>>(data)) {}

  // ��ȡ��data_����
  template <typename T>
  T cast_() {
    // ����ָ��---��������ָ��
    Derive<T>* pd = dynamic_cast<Derive<T>>(base_.get());
    if (pd == nullptr) {
      throw "type is unmatch!";
    }
    return pd->data_;
  }

 private:
  class Base {
   public:
    virtual ~Base() = default;
  };

  template <typename T>
  class Derive : public Base {
   public:
    Derive(T data) : data_(data) {}
    T data_;  //�������������������
  };

 private:
  // ����һ������ָ��
  std::unique_ptr<Base> base_;
};

// ʵ��һ���ź�����
class Semaphore {
 public:
  Semaphore(int limit = 0) : resLimit_(limit) {}
  ~Semaphore() = default;

  // ��ȡһ���ź�����Դ
  void wait() {
    std::unique_lock<std::mutex> lock(mtx_);
    cond_.wait(lock, [&]() -> bool { return resLimit_ > 0; });
    resLimit_--;
  }

  // ����һ���ź�����Դ
  void post() {
    std::unique_lock<std::mutex> lock(mtx_);
    resLimit_++;
    cond_.notify_all();
  }

 private:
  int resLimit_;
  std::mutex mtx_;
  std::condition_variable cond_;
};

class Task;  // Task����ǰ������
// ʵ�ֽ����ύ���̳߳ص�task����ִ����ɺ�ķ���ֵ����Result
class Result {
 public:
  Result(std::shared_ptr<Task> task, bool isValid = true);
  ~Result() = default;

  // setVal��������ȡ����ִ����ķ���ֵ��
  void setVal(Any any);
  // get������ �û��������������ȡTask�ķ���ֵ
  Any get();

 private:
  Any any_;                     // �洢����ķ���ֵ
  Semaphore sem_;               // �߳�ͨ���ź���
  std::shared_ptr<Task> task_;  // ָ���Ӧ��ȡ����ֵ���������
  std::atomic_bool isValid_;    //����ֵ�Ƿ���Ч
};

//����������
class Task {
 public:
  Task() : result_(nullptr) {}
  // �û������Զ��������������ͣ���Task�̳У���дrun��������ʵ���Զ���������
  virtual Any run() = 0;
  void exec();
  void setResult(Result* res) { 
	if (result_ != nullptr) {
      result_ = res;
    } 
  }

 private:
  Result* result_;  // Result�������ڳ���Task��
};

// �̳߳�֧�ֵ�ģʽ
enum class PoolMode {
  MODE_FIXED,   //�̶������߳�
  MODE_CACHED,  //�߳������ɶ�̬����
};

//�߳�����
class Thread {
 public:
  // �̺߳�����������
  using ThreadFunc = std::function<void()>;
  //�̹߳���
  Thread(ThreadFunc func);
  //�߳�����
  ~Thread();
  // �����߳�
  void start();

 private:
  ThreadFunc func_;
};

// �̳߳�����
class ThreadPool {
 public:
  ThreadPool();
  ~ThreadPool();

  void start(int initThreadSize = 4);

  void setMode(PoolMode mode);

  //���ó��Ե��߳�����
  void setInitThreadSize(int size);

  // ����task�������������ֵ
  void setTaskQueMaxThreadHold(int threshhold);

  // ���̳߳��ύ����
  Result submitTask(std::shared_ptr<Task> sp);

  // ��ϣ���û����俽���븳ֵ����
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

 private:
  std::vector<std::unique_ptr<Thread>> threads_;  //�߳��б�
  size_t initThreadSize_;                         //���Ե��߳�����
  int threadSizeThreshHold_;                      // �߳�����������ֵ

  // ����Ҫִ������Ļ����Զ������ˣ���ʱ������׳���
  std::queue<std::shared_ptr<Task>> taskQue_;
  // ԭ�Ӳ����Ӽ�û��Ҫֱ������
  std::atomic_uint taskSize_;
  int taskQueMaxThreadHold_;  // �����������1��ֵ

  std::mutex taskQueMtx_;  //��֤������е��̰߳�ȫ
  // TODO ��������
  std::condition_variable notFull_;   //��ʾ������в���
  std::condition_variable notEmpty_;  //��ʾ������в���

  PoolMode poolMode_;  //��ǰ�̳߳صĹ���ģʽ
 private:
  // �����̺߳���
  void threadFunc();
};

#endif  // !THREADPOOL_H
