// #program once ��vsƽ̨�����ã�Linuxƽ̨�¿���ʧЧ
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>
#include<functional>
using namespace std;

//����������
class Task {
 public:
  // �û������Զ��������������ͣ���Task�̳У���дrun��������ʵ���Զ���������
  virtual void run() = 0;
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
  void submitTask(std::shared_ptr<Task> sp);

  // ��ϣ���û����俽���븳ֵ����
  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;

 private:
  std::vector<std::unique_ptr<Thread>> threads_;  //�߳��б�
  size_t initThreadSize_;         //���Ե��߳�����
  int threadSizeThreshHold_;      // �߳�����������ֵ

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
