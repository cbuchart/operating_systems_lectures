#include <iostream>
#include <string>

int main() {
  std::cout << "<html>\n";

  std::string line;
  while (getline(std::cin, line)) {
    std::cout << line << "<br>\n";
  }

  std::cout << "</html>\n";
}
