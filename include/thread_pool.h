#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

#include "logger.h"

struct Task {
  std::function<void()> op;
  int taskId;
  Task(std::function<void()> &&, int);
};

class ThreadPool {
private:
  std::queue<Task> taskQueue;
  std::vector<std::thread> threads;

  int status;
  std::mutex statusLock;

  void worker();

public:
  ThreadPool(int);
  ~ThreadPool();

  int GetStatus();
  void Shutdown();

  template <typename F, typename... Args>
  auto SubmitTask(int taskId, F &&f, Args &&...args)
      -> std::future<typename std::invoke_result<F, Args...>::type>;

  ThreadPool(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ThreadPool &operator=(ThreadPool &&) = delete;
};

template <typename F, typename... Args>
auto ThreadPool::SubmitTask(int taskId, F &&f, Args &&...args)
    -> std::future<typename std::invoke_result<F, Args...>::type> {
  using ReturnType = typename std::invoke_result<F, Args...>::type;

  // 持久化 task
  auto task = std::make_shared<std::packaged_task<ReturnType()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<ReturnType> res = task->get_future();

  opWithMutex(statusLock, [this, task, taskId]() {
    if (this->status == STATUS_STOPPED) {
      throw std::runtime_error("Thread pool has stopped");
    }
    this->taskQueue.emplace([task]() { (*task)(); }, taskId);
  });
  Logger::info("Submit a new task, id=", taskId);

  return res;
}