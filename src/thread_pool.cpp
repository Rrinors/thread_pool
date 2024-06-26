#include "../include/thread_pool.h"

Task::Task(std::function<void()> &&__op, int __taskId) {
  op = __op;
  taskId = __taskId;
}

void ThreadPool::worker() {
  while (GetStatus() == STATUS_WORKING) {
    auto [task, taskId, ok] = opWithMutex(statusLock, [this]() {
      if (this->taskQueue.empty()) {
        return std::make_tuple(std::function<void()>{}, 0, false);
      }
      auto [task, id] = this->taskQueue.front();
      this->taskQueue.pop();
      return std::make_tuple(task, id, true);
    });

    if (!ok) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      continue;
    }

    auto future = std::async(std::launch::async, task);

    try {
      future.get();
      Logger::info("Task processed success, id=", taskId);
    } catch (const std::exception &e) {
      Logger::error("Task processed error, id=", taskId, ", err=", e.what());
    }
  }
}

ThreadPool::ThreadPool(int threadCount) {
  status = STATUS_STOPPED;
  Start(threadCount);
}

int ThreadPool::GetStatus() {
  return opWithMutex(statusLock, [this]() { return this->status; });
}

void ThreadPool::Shutdown() {
  auto cnt = opWithMutex(statusLock, [this]() {
    this->status = STATUS_STOPPED;
    return this->taskQueue.size();
  });
  for (auto &thread : threads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  threads.clear();
  Logger::info("Thread pool shutdown, ", cnt, " task in queue");
}

ThreadPool::~ThreadPool() { Shutdown(); }

void ThreadPool::Start(int threadCount) {
  opWithMutex(statusLock, [this, threadCount]() {
    if (this->status == STATUS_WORKING) {
      Logger::info("Thread pool has started");
      return;
    }
    this->status = STATUS_WORKING;
    for (int i = 0; i < threadCount; i++) {
      threads.emplace_back(&ThreadPool::worker, this);
    }
    Logger::info("Thread pool start");
  });
}