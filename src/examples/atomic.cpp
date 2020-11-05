#include <chrono>
#include <iostream>
#include <thread>
#include <string>
#include <atomic>

#pragma region TICTOC
// Class to measure the execution time of a block
class TicToc
{
  std::chrono::time_point<std::chrono::steady_clock> m_begin;
  std::string m_label;

public:
  explicit TicToc(const std::string &label) : m_begin(std::chrono::high_resolution_clock::now()), m_label(label) {}

  ~TicToc()
  {
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_begin).count();
    std::clog << m_label << duration / 1000.0 << "ms\n";
  }
};
#define HELPER_JOIN(a, b) HELPER_JOIN2(a, b)
#define HELPER_JOIN2(a, b) a##b

// Helper macro to automate the creation of timers
#define TICTOC()                                                                                                       \
  TicToc HELPER_JOIN(tic_toc_, __LINE__) { __FUNCTION__ " " }
#pragma endregion

int g_value = 0; // no protection
// std::atomic<int> g_value = 0; // atomic

void inc(size_t count)
{
  for (size_t ii = 0; ii < count; ++ii) { ++g_value; }

  // std::atomic cannot be used here
  /*for (size_t ii = 0; ii < count; ++ii) {
    int g = g_value;
    ++g;
    g_value = g;
  }*/
}

void dec(size_t count)
{
  for (size_t ii = 0; ii < count; ++ii) { --g_value; }
}

int main()
{
  TICTOC();

  std::thread inc_th(inc, 1'000'000);
  std::thread dec_th(dec, 1'000'000);

  inc_th.join();
  dec_th.join();

  std::cout << g_value << '\n';
}
