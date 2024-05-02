#include <mutex>

template <typename Op, typename... Args>
decltype(auto) opWithMutex(std::mutex &mtx, Op &&op, Args &&...args);

template <typename Op, typename... Args>
decltype(auto) opWithMutex(std::mutex &mtx, Op &&op, Args &&...args) {
  std::unique_lock<std::mutex> lock(mtx);
  return std::forward<Op>(op)(std::forward<Args>(args)...);
}