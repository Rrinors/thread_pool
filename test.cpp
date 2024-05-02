#include "include/thread_pool.h"

int f(int x) {
  std::this_thread::sleep_for(std::chrono::seconds(5));
  return x;
}

int main() {
  ThreadPool tp(4);

  std::vector<std::future<int>> res;
  for (int i = 0; i < 20; i++) {
    res.emplace_back(tp.SubmitTask(i, f, i));
  }

  std::this_thread::sleep_for(std::chrono::seconds(10));

  tp.Shutdown();
  tp.Start(8);

  for (auto &r : res) {
    Logger::info("Task done: ", r.get());
  }

  return 0;
}