// #program once ��vsƽ̨�����ã�Linuxƽ̨�¿���ʧЧ
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <memory>
#include <queue>
#include <vector>
#include <atomic>
#include <mutex>
#include <condition_variable>
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
 private:
};

// �̳߳�����
class ThreadPool {
 public:
  ThreadPool();
  ~ThreadPool();

  void start();

  void setMode(PoolMode mode);


 private:
  std::vector<Thread*> threads_;  //�߳��б�
  size_t initThreadSize_;         //���Ե��߳�����

  // ����Ҫִ������Ļ����Զ������ˣ���ʱ������׳���
  std::queue<std::shared_ptr<Task>> taskQue_;
  // ԭ�Ӳ����Ӽ�û��Ҫֱ������
  std::atomic_uint taskSize;
  int taskQueMaxThreadHold_;  // �����������1��ֵ

  std::mutex taskQueMtx_; //��֤������е��̰߳�ȫ
  //TODO ��������
  std::condition_variable notFull_; //��ʾ������в���
  std::condition_variable notEmpty_;  //��ʾ������в���

  PoolMode poolMode_; //��ǰ�̳߳صĹ���ģʽ
};

#endif  // !THREADPOOL_H
