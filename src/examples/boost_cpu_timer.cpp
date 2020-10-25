#include <boost/timer/timer.hpp>
#include <iostream>
#include <cmath>

double run(size_t count) {
  boost::timer::auto_cpu_timer cpu_timer;

  auto sum = 0.0;

  for (size_t ii = 0; ii < count; ++ii) {
    if (ii % 1'000 == 0) {
      std::cout << ii << '\n';
    }
    sum += cos(ii);
  }

  return sum;
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " <count>\n";
    return 1;
  }
  
  std::cout << run(atoi(argv[1])) << '\n';

  return 0;
}
