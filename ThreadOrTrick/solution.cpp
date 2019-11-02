#include <iostream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <random>
#include <atomic>

constexpr size_t g_child_count = 10;
constexpr size_t g_house_count = 20;

constexpr size_t g_treat_limit = 100;
constexpr size_t g_trick_limit = 100;

constexpr size_t g_candy_limit_per_house = 10;

struct House {
  size_t candies = g_candy_limit_per_house;

  std::mutex mutex;
};
House g_houses[g_house_count];

// Global stop indicator
std::atomic<bool> g_limit_reached = false;

struct Statistics {
  size_t treat_count = 0;
  size_t trick_count = 0;
  double treat_avg = 0.0;
  double trick_avg = 0.0;

  std::mutex mutex;
} g_statistics;

void child(size_t next_house)
{
  constexpr auto treat_duration = std::chrono::milliseconds(10);
  constexpr auto trick_duration = std::chrono::milliseconds(50);
  constexpr auto travel_duration = std::chrono::milliseconds(20);

  size_t treats = 0;
  size_t tricks = 0;

  while (!g_limit_reached) {
    if (true) { // enter house and wait to be attended
      std::lock_guard<std::mutex> lock(g_houses[next_house].mutex);

      if (g_houses[next_house].candies > 0) {
        ++treats;
        --g_houses[next_house].candies;
        std::this_thread::sleep_for(treat_duration);
      } else {
        ++tricks;
        std::this_thread::sleep_for(trick_duration);
      }
    }

    // Check stop condition
    if (treats == g_treat_limit || tricks == g_trick_limit) {
      g_limit_reached = true;
      break;
    }

    // Go to the next house
    std::this_thread::sleep_for(travel_duration);
    next_house = (next_house + 1) % g_house_count;
  }

  // Accumulate results once the party has ended
  std::lock_guard<std::mutex> lock(g_statistics.mutex);
  g_statistics.treat_count += treats;
  g_statistics.trick_count += tricks;
}

void deliveryman()
{
  constexpr auto delivery_duration = std::chrono::milliseconds(5);
  constexpr auto travel_duration = std::chrono::milliseconds(15);

  constexpr size_t low_threshold = 3;
  constexpr size_t single_charge = 10;

  size_t next_house = 0;

  while (!g_limit_reached) {
    if (true) { // enter house and wait to be attended
      std::lock_guard<std::mutex> lock(g_houses[next_house].mutex);
      if (g_houses[next_house].candies < low_threshold) {
        g_houses[next_house].candies = std::min(g_houses[next_house].candies + single_charge, g_candy_limit_per_house);
        std::this_thread::sleep_for(delivery_duration);
      }
    }

    // Go to the next house
    std::this_thread::sleep_for(travel_duration);
    next_house = (next_house + 1) % g_house_count;
  }
}

int main()
{
  // Random number generator
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(0, g_house_count - 1);

  // Threads
  std::thread deliveryman_threads(deliveryman);
  std::vector<std::thread> child_threads;
  for (size_t ii = 0; ii < g_child_count; ++ii) {
    child_threads.emplace_back(child, dist(mt)); // start at random house
  }

  // Wait for party to end
  deliveryman_threads.join();
  for (auto& child_thread : child_threads) {
    child_thread.join();
  }

  // Compute and display statistics
  g_statistics.treat_avg = g_statistics.treat_count / (double)g_child_count;
  g_statistics.trick_avg = g_statistics.trick_count / (double)g_child_count;

  std::cout << "STATISTICS\n";
  std::cout << "Total treats: " << g_statistics.treat_count << "\n";
  std::cout << "Avg. treats:  " << g_statistics.treat_avg << "\n";
  std::cout << "Total tricks: " << g_statistics.trick_count << "\n";
  std::cout << "Avg. tricks:  " << g_statistics.trick_avg << "\n";

  size_t remaining_candies = 0;
  for (const auto& house : g_houses) {
    remaining_candies += house.candies;
  }
  std::cout << "Rem. candies: " << remaining_candies << "\n";

  return 0;
}
