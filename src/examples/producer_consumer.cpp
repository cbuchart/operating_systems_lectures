#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <random>
#include <list>

using namespace std::chrono_literals;

size_t randomNumber(size_t max)
{
  std::random_device rd;
  std::mt19937 mt(rd());                           // Mersenne Twister 19937 generator
  std::uniform_int_distribution<int> dist(0, max); // [0, max]

  return dist(mt);
}

std::atomic<bool> g_stop = false;          // trigger to stop threads execution
std::atomic<size_t> g_running_threads = 0; // used to keep track of number of threads still running

std::list<size_t> g_items; // unbounded buffer for elements
std::mutex g_items_mutex;
std::condition_variable g_items_cv;

std::mutex g_cout_mutex;

void producer()
{
  while (!g_stop) {
    std::this_thread::sleep_for(1ms);

    std::unique_lock lock(g_items_mutex);
    g_items.push_back(randomNumber(1'000));
    g_items_cv.notify_one();
  }

  std::unique_lock cout_lock(g_cout_mutex);
  std::cout << "Producer ended!\n";
}

void consumer(size_t id)
{
  ++g_running_threads;

  while (true) {
    size_t item, size;
    { // Critical section
      std::unique_lock lock(g_items_mutex);
      g_items_cv.wait(lock);

      // Can stop only if all items have been consumed
      if (g_stop && g_items.empty()) { break; }

      item = g_items.front();
      g_items.pop_front();
      size = g_items.size();
    }

    std::unique_lock cout_lock(g_cout_mutex);
    std::cout << "Consumer " << id << " -> " << item << " (" << size << ")\n";
  }

  --g_running_threads;

  std::unique_lock cout_lock(g_cout_mutex);
  std::cout << "Consumer " << id << " ended!\n";
}

int main()
{
  std::thread producer_thread(producer);
  std::vector<std::thread> consumer_threads;
  for (size_t ii = 0; ii < 10; ++ii) { consumer_threads.emplace_back(consumer, ii); }

  std::this_thread::sleep_for(5'000ms);

  g_stop = true;
  while (g_running_threads > 0) { g_items_cv.notify_all(); }

  producer_thread.join();
  for (auto &thread : consumer_threads) { thread.join(); }

  std::cout << "Items remaining -> " << g_items.size() << '\n';

  return 0;
}
