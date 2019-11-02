# Thread or Trick

## Introduction
Tonight is Halloween, children are excited about tonight's party: costumes, candies and, if there is no other choice, settle accounts with those wicked who don't have enough sweet supplies. And that is that year after year the thing has got worse: more little monsters dressed as children, less candies.

You've been chosen to try to prevent the disaster by running a simulation of what can happen and be prepared.

## Problem
Develop an application that can simulate the party and obtain statistics about candy flow and trick count.

To do so, you must take into consideration the following parameters:

-   There are ```N``` children in the neighborhood.
-   There are ```M``` houses in the neighborhood.
-   ```N, M > 0```.
-   No house will have more than ```C=10``` candies at any moment.
-   Each house will start with maximum candy supplies.
-   When a child knocks on a door it will ask for "Treat or trick". If there are candies at the house, will be a treat (decreasing house's stack and increasing child's count). Otherwise trick.
-   Trick duration is ```50ms```, while treating is ```10ms```. Moving from one house to the next one takes ```20ms```.
-   Each child may start at any house of the neighborhood.
-   There is a candy delivery man that is constantly giving a few candies (```D=10```) to each house if it is running below a certain threshold ```T<=3```. The delivery man has infinite candy supplies.
-   Delivery is done in ```5ms``` and it takes ```15ms``` to reach the next house.
-   The delivery man always start at the first house.
-   While the owner of a house is busy attending a child no other child or the delivery man can access the candy storage. It includes the case of trick. The same happens when the delivery man is giving candies: no child can access such house and must wait.
-   Both children and delivery man travel houses in the same increasing order. If last house is reached, the next house is 0.
-   Both children and delivery man will be traveling houses over and over until either a child reaches 100 candies or 100 tricks done. Candies and tricks count are checked before moving to the next house.

At the end, overall statistics must be printed:
-   Total number of candies received.
-   Average number of candies received per child.
-   Total number of tricks done.
-   Average number of tricks done per child.
-   Total number of candies remaining at houses.
 
## Tips
-   Identify the actors in the problem that has to perform actions in parallel.
-   Identify the resources that may be accessed by more than one actor and determine if a race condition may occur.
-   Some threading problems can be solved first in a serial fashion and then be migrated to a parallel solution. For this problem this approach is not recommended, given the concurrent nature of children and the delivery man.
-   As a general programming consideration, use constants for any parameter, even when it seems to be fixed (for example, durations, number of children, maximum candy storage...). In this way it will be easier for you to make modifications and also experiment with the parameters.

## Delivery and Grading
Solution proposed shall be developed using C++11 or higher, all into a single ```.cpp``` file (no compressed solutions, no executables, etc).

Source code must be sent to Professor's email before October 31st, 2019, 23:59:59. Only the last email within the valid period will be corrected, but all deliveries after deadline will just be ignored.

This practice aims for 5% of the course (maximum score is then 0.5).

Grading rules are as follows:
-   No code sent: 0 points. Code sent after deadline will be ignored and doesn't count at all.
-   Any additional file other than single ```.cpp``` source code file: -0.1 points.
-   Code doesn't compile: 0 points.
-   No statistics: -0.1 points.
-   Race conditions are not handled correctly: -0.25 points.
-   Excesive protection that leads to low concurrency levels: -0.1 points.
-   No multi-threading at all: -0.45 points.

## Additional questions
Following question will help you to delve into the topic. Though they are not graded, it is recommended to think about them.

-   What computer-related situations can be compared with this problem? Associate each actor and action.
-   What does happen when the number of children is smaller than the number of houses? And when it is greater? Or fives times as large?
-   What are the effects of modifying the parameters of the delivery man? And supressing it?

## Initial code template

Code below can be used as a starting point for your project:

```c++
#include <iostream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <vector>
#include <cassert>
#include <random>
#include <atomic>

constexpr size_t g_child_count = 10;
constexpr size_t g_house_count = 20;

constexpr size_t g_treat_limit = 100;
constexpr size_t g_trick_limit = 100;

constexpr size_t g_candy_limit_per_house = 10;

void child(size_t next_house)
{
  constexpr auto travel_duration = std::chrono::milliseconds(20);

  while (true) {
    std::this_thread::sleep_for(travel_duration);
  }
}

void deliveryman()
{
  constexpr auto travel_duration = std::chrono::milliseconds(15);

  while (true) {
    std::this_thread::sleep_for(travel_duration);
  }
}

int main()
{
  // Random number generator
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(0, g_house_count - 1);

  // Thread creation
  std::vector<std::thread> child_threads;
  for (size_t ii = 0; ii < g_child_count; ++ii) {
    child_threads.emplace_back(child, dist(mt)); // start at random house
  }

  // ...

  // Results
  std::cout << "STATISTICS\n";
  std::cout << "Total treats: " << "\n";
  std::cout << "Avg. treats:  " << "\n";
  std::cout << "Total tricks: " << "\n";
  std::cout << "Avg. tricks:  " << "\n";
  std::cout << "Rem. candies: " << "\n";

  return 0;
}
```

## Code snippets

### Thread creation
```c++
void a_function(int a_parameter) { /* ... */ }

std::thread a_thread(a_function, 5); // starts thread immediately
```

### Waiting for a thread to finish
```c++
a_thread.join();
```

### Using a mutex
```c++
std::mutex a_mutex;

a_mutex.lock();
// ...
a_mutex.unlock();
```

Or

```c++
{
  std::lock_guard<std::mutex> lock(a_mutex);
  // ...
}
```

### Sleeping a thread
```c++
std::this_thread::sleep_for(std::chrono::milliseconds(42));
```

### Generating random numbers
```c++
std::random_device rd;
std::mt19937 mt(rd()); // Mersenne Twister 19937 generator
std::uniform_int_distribution<int> dist(0, 100); // uniform distribution in [0, 100]
```

## Further interesting reading
-   Atomic variables
-   ```constexpr```
-   ```std::endl``` vs ```'\n'```
-   ```size_t```
