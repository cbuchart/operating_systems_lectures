#include <chrono>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include <mutex>

// Generates a random number with uniform distribution in [0, max]
size_t randomNumber(size_t max)
{
  std::random_device rd;
  std::mt19937 mt(rd()); // Mersenne Twister 19937 generator
  std::uniform_int_distribution<int> dist(0, max);

  return dist(mt);
}

// Mutex used to protect std::cout from race conditions
std::mutex g_cout_mutex;

void worker(size_t id)
{
  // Force higher differences in the start of each thread
  std::this_thread::sleep_for(std::chrono::milliseconds(randomNumber(10)));

  // Critical section for std::cout
  std::unique_lock lock(g_cout_mutex);
  // std::this_thread::get_id() is dependant on the operating system implementation
  std::cout << "Hello from " << id << " in thread " << std::this_thread::get_id() << "\n";
}

int main()
{
  // Create threads
  std::vector<std::thread> threads;
  // emplace_back(worker, ii) is similar to push_back(std::thread(worker, ii)), but constructs the object directly in vector's memory, thus is a bit faster
  for (size_t ii = 0; ii < 10; ++ii) { threads.emplace_back(worker, ii); }

  // Using C++11 lambda functions
  std::thread t3([]() { std::cout << "Hello from " << std::this_thread::get_id() << " in " << __FUNCTION__ << '\n'; });

  { // Critical section for std::cout, isolated from the rest of the code block
    std::unique_lock lock(g_cout_mutex);
    std::cout << "Hello from main " << std::this_thread::get_id() << "\n";
  }

  // Wait for threads to finish
  for (auto &t : threads) { t.join(); }
  t3.join();
  
  return 0;
}
