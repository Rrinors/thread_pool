#include "include/thread_pool.h"

int f(int x) {
  return x;
}

int main() {
  ThreadPool tp(4);

  std::vector<std::future<int>> res;
  for (int i = 0; i < 20; i++) {
    res.emplace_back(tp.SubmitTask(i, f, i));
  }

  for (int i = 0; i < 20; i++) {
    int val = res[i].get();
    Logger::info("Task Done: ", val);
  }

  tp.Shutdown();

  return 0;
}