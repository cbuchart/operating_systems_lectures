// +
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <unordered_set>
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <list>
#include <vector>
#include <array>
#include <future>

// Word search method:
// - TRIE = true -> uses a trie data structure
// - TRIE = false -> uses a set of strings
#define USE_TRIE() true

// Four different approaches
#define SERIAL 0              // straightforward solution: no parallelization at all
#define LIMITED_CONSUMERS 1   // number of consumers (checker threads) limited to core-count
#define UNLIMITED_CONSUMERS 2 // unlimited number of threads
#define ASYNC_TASKS 3         // using async_task from std library
#define METHOD() ASYNC_TASKS

namespace fs = std::filesystem;
using namespace std::string_literals;
using namespace std::chrono_literals;

// Helper functions
namespace
{
  // Measures time spent on a certain block
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

  // Some macros to ease the use of the TicToc class
#define HELPER_JOIN(a, b) HELPER_JOIN2(a, b)
#define HELPER_JOIN2(a, b) a##b
#define TICTOC()                                                                                                       \
  TicToc HELPER_JOIN(tic_toc_, __LINE__) { __FUNCTION__ " " }

  void toLower(std::string &str) { std::transform(begin(str), end(str), begin(str), ::tolower); }

  enum class TokenType {
    Word,      // here a word is simply something that is not a separator
    Separator, // separators limit words: space, punctuation signs...
  };

  struct Token
  {
    std::string value;
    TokenType type;

    Token(std::string value, TokenType type) : value{std::move(value)}, type{type} {}
  };

  // Split a string into a vector of tokens (words or separators)
  std::vector<Token> split(const std::string &str, const std::string &seps)
  {
    std::vector<Token> tokens;
    for (size_t ii = 0; ii < str.size();) {
      const auto first = str.find_first_not_of(seps, ii);
      if (first == std::string::npos) {
        tokens.emplace_back(str.substr(ii), TokenType::Separator);
        break;
      }
      if (first > ii) { tokens.emplace_back(str.substr(ii, first - ii), TokenType::Separator); }

      const auto last = str.find_first_of(seps, first + 1);
      if (last == std::string::npos) {
        tokens.emplace_back(str.substr(ii), TokenType::Word);
        break;
      }
      tokens.emplace_back(str.substr(first, last - first), TokenType::Word);

      ii = last;
    }

    return tokens;
  }
}

namespace
{
#if USE_TRIE()
  // A trie is a tree with a fixed number of children, each of them corresponding with a letter of the alphabet
  // The interface of Dictionary class is so that is compatible with std::set and std::unordered_set, allowing a
  // seamless change between them
  struct Dictionary
  {
    struct Level
    {
      std::array<std::unique_ptr<Level>, 'z' - 'a' + 1> level;
      bool is_word = false;

      // Note: do not operate on std::string but on char pointers that are faster for insertion and search, since no new
      // strings are created
      void insert(const char *str)
      {
        auto ptr = this;
        while (*str) {
          if (!ptr->level[*str - 'a']) { ptr->level[*str - 'a'] = std::make_unique<Level>(); }
          ptr = ptr->level[*str - 'a'].get();
          ++str;
        }
        ptr->is_word = true;
      }

      bool contains(const char *str) const
      {
        auto ptr = this;
        while (*str) {
          ptr = ptr->level[*str - 'a'].get();
          if (!ptr) { return false; }
          ++str;
        }
        return ptr->is_word;
      }
    } root;

    void insert(const std::string &word) { root.insert(word.c_str()); }

    bool contains(const std::string &word) const { return root.contains(word.c_str()); }
  };
#else
  using Dictionary = std::unordered_set<std::string>;
#endif

  // Dictionary-related variables are globals to simplify logic (dictionaries are read-only on workers)
  Dictionary g_dictionary;
  const auto g_separators = " \t.,;:\"?!-"s;

  void loadDictionary(const fs::path &path)
  {
    TICTOC();

    std::ifstream file{path};
    if (!file.is_open()) { return; }

    std::string word;
    while (getline(file, word)) {
      toLower(word); // normalize all words
      g_dictionary.insert(word);
    }
  }

  bool isWord(const std::string &word)
  {
    const auto alpha_count = std::count_if(cbegin(word), cend(word), ::isalpha);

    if (alpha_count == word.size()) { return true; }

    // Is it a contraction?
    if (alpha_count == word.size() - 1) { return std::count(cbegin(word), cend(word), '\'') == 1; }

    return false;
  }

  bool isNumber(const std::string &word)
  {
    const auto digit_count = std::count_if(cbegin(word), cend(word), ::isdigit);

    // Integer number
    if (digit_count == word.size()) { return true; }

    // Check for number with decimal point
    if (digit_count == word.size() - 1) {
      if (std::count(cbegin(word), cend(word), '.') == 1) { return true; }
    }

    return false;
  }

  bool existInDictionary(const std::string &o_word, const Dictionary &dict)
  {
    auto word = o_word;

    // Check for contractions
    // Another option would be to include them in the dictionary, but for the case of a trie then 27 children per level
    // are needed instead of 26
    // Nevertheless posessives must be checked separately
    if (const auto pos = word.find('\'', 0); pos != std::string::npos) {
      if (word.size() < 3) { return false; } // no contraction possible

      switch (word[0]) { // check first letter to avoid unnecessary conditionals
      case 'i':
        if (word == "i'm") { return true; }
        if (word == "it's") { return true; }
        if (word == "i'd") { return true; }
        if (word == "it'd") { return true; }
        if (word == "i'll") { return true; }
        if (word == "it'll") { return true; }
        if (word == "i've") { return true; }
        break;
      case 'y':
        if (word == "you're") { return true; }
        if (word == "you'd") { return true; }
        if (word == "you'll") { return true; }
        if (word == "you've") { return true; }
        break;
      case 'h':
        if (word == "he's") { return true; }
        if (word == "he'd") { return true; }
        if (word == "he'll") { return true; }
        if (word == "hasn't") { return true; }
        if (word == "haven't") { return true; }
        if (word == "hadn't") { return true; }
        break;
      case 's':
        if (word == "she's") { return true; }
        if (word == "she'd") { return true; }
        if (word == "she'll") { return true; }
        break;
      case 't':
        if (word == "that's") { return true; }
        if (word == "they're") { return true; }
        if (word == "they'd") { return true; }
        if (word == "they'll") { return true; }
        if (word == "they've") { return true; }
        break;
      case 'w':
        if (word == "we'd") { return true; }
        if (word == "we'll") { return true; }
        if (word == "we've") { return true; }
        if (word == "wasn't") { return true; }
        if (word == "weren't") { return true; }
        if (word == "won't") { return true; }
        break;
      case 'c':
        if (word == "couldn't") { return true; }
        if (word == "can't") { return true; }
        break;
      case 'd':
        if (word == "don't") { return true; }
        if (word == "doesn't") { return true; }
        if (word == "didn't") { return true; }
        break;
      }

      if (pos == word.size() - 1 || (pos == word.size() - 2 && word.back() == 's')) {
        // Check for possesive
        word = word.substr(0, pos);
      }
    }

#if USE_TRIE()
    return dict.contains(word);
#else
    return dict.find(word) != dict.end();
#endif
  }

  // Structure used to store information about a line
  struct Line
  {
    size_t line_number;
    std::string str;

    Line() = default;
    Line(size_t line_number, std::string str) : line_number{line_number}, str{std::move(str)} {}

    // In case we need to sort lines
    bool operator<(const Line &other) const { return line_number < other.line_number; }
  };

  // Function to check and highlight errors in a single line
  Line checkLine(const Line &line)
  {
    std::stringstream sstr;
    const auto tokens = split(line.str, g_separators);

    for (const auto &token : tokens) {
      if (token.type == TokenType::Word) {
        auto word = token.value;
        toLower(word);

        bool misspelled = false;
        if (isWord(word)) {
          misspelled = !existInDictionary(word, g_dictionary);
        } else {
          // Remember that numbers are not misspelled
          misspelled = !isNumber(word);
        }

        if (misspelled) {
          std::string suggestion;

          // Find suggestion here (not implemented)

          if (suggestion.empty()) {
            sstr << R"(<a style="color:red">)";
          } else {
            sstr << R"(<a href=" " title=")" << suggestion
                 << R"(" style="background-color:#FFFFFF;color:red;text-decoration:underline">)";
          }
        }

        sstr << token.value;

        if (misspelled) { sstr << "</a>"; }
      } else {
        sstr << token.value;
      }
    }

    return {line.line_number, sstr.str()};
  }

#if METHOD() == SERIAL
  void readWrite(const Line &line)
  {
    const auto checked_line = checkLine(line);
    std::cout << checked_line.str << "<br>\n";
  }
#else
  std::mutex g_input_mutex;
  std::condition_variable g_input_cv;
  std::list<Line> g_input;

  std::mutex g_output_mutex;
  std::condition_variable g_output_cv;
  std::list<Line> g_output;
  constexpr size_t g_output_flush_size = 10;

  std::atomic<bool> g_stop_reading = false;
  std::atomic<bool> g_stop_writing = false;

  void queuedSyncCheckLine(const Line &line)
  {
    const auto checked_line = checkLine(line);

    std::unique_lock lock(g_output_mutex);
    g_output.emplace_back(checked_line);
    if (g_output.size() > g_output_flush_size) {
      lock.unlock();
      g_output_cv.notify_one();
    }
  }

  void reader()
  {
    Line line;

    while (true) {
      if (true) { // critical section
        std::unique_lock lock(g_input_mutex);
#  if METHOD() == LIMITED_CONSUMERS
        g_input_cv.wait(lock, []() { return !g_input.empty() || g_stop_reading; });
        if (g_input.empty() && g_stop_reading) { break; }
#  endif

        // Extract line but process it when g_input is not blocked
        std::swap(line, g_input.front());
        g_input.pop_front();
      }

      queuedSyncCheckLine(line);
    }
  }

  void writer()
  {
    size_t next_line = 0;
    std::vector<std::string> lines;

    std::cout << "<html>\n";

    while (true) {
      if (true) { // critical section
        std::unique_lock lock(g_output_mutex);
        g_output_cv.wait(lock, []() { return !g_output.empty() || g_stop_writing; });
        if (g_output.empty() && g_stop_writing) { break; }

        // Grab next lines, in order
        g_output.sort();
        while (!g_output.empty() && g_output.front().line_number == next_line) {
          // Defer printing to stdout to avoid blocking g_output due to I/O delay
          lines.emplace_back(std::move(g_output.front().str));
          g_output.pop_front();

          ++next_line;
        }
      }

      // Print to stdout when g_output is not blocked
      for (const auto &line : lines) { std::cout << line << "<br>\n"; }
      lines.clear();
    }

    std::cout << "</html>\n";

    std::clog << "Wrote " << next_line << " lines\n";
  }
#endif
}

int main(int argc, char *argv[])
{
  TICTOC();

  if (argc < 2) {
    std::cerr << "Invalid number of files. Usage " << argv[0] << " <dictionary_file> [<number_of_threads>]\n";
    return 1;
  }

  std::cout.sync_with_stdio(false);
  std::cin.sync_with_stdio(false);

  loadDictionary(argv[1]);

#if METHOD() != SERIAL
  // Prepare worker threads
  std::thread writer_thread{writer};
  std::vector<std::thread> threads;

#  if METHOD() == LIMITED_CONSUMERS
  const size_t thread_count = argc >= 3 ? atoi(argv[2]) : std::thread::hardware_concurrency() - 2;
  std::clog << "Using " << thread_count << " threads\n";
  threads.reserve(thread_count);
  for (size_t ii = 0; ii < threads.capacity(); ++ii) { threads.emplace_back(reader); }
#  elif METHOD() == ASYNC_TASKS
  std::vector<std::future<void>> tasks;
#  endif
#endif

  // Read input text
  size_t line_number = 0;
  std::string line;
  while (getline(std::cin, line)) {
#if METHOD() == SERIAL
    readWrite({line_number++, line});
#else
    if (true) {
#  if METHOD() == LIMITED_CONSUMERS
      std::unique_lock lock(g_input_mutex);
      g_input.emplace_back(line_number++, line);
#  elif METHOD() == UNLIMITED_CONSUMERS
      threads.emplace_back(queuedSyncCheckLine, Line{line_number++, line});
#  elif METHOD() == ASYNC_TASKS
      tasks.push_back(std::async(queuedSyncCheckLine, Line{line_number++, line}));
#  endif
    }

#  if METHOD() == LIMITED_CONSUMERS
    g_input_cv.notify_all();
#  endif
#endif
  }
  std::clog << "Read " << line_number << " lines\n";

#if METHOD() != SERIAL
  // Wait until all readers have finished
  std::clog << "Wait for readers to empty buffers...\n";
#  if METHOD() == LIMITED_CONSUMERS
  g_stop_reading = true;
  while (true) {
    if (true) {
      std::unique_lock lock(g_input_mutex);
      if (g_input.empty()) { break; }
    }
    g_input_cv.notify_all();
  }
#  endif

#  if METHOD() != ASYNC_TASKS
  std::clog << "Wait for readers (threads) to join...\n";
  for (auto &&thread : threads) {
    g_input_cv.notify_all();
    thread.join();
  }
#  else
  std::clog << "Wait for readers (tasks) to join...\n";
  for (auto &&task : tasks) { task.wait(); }
#  endif

  // Wait for writer to finish
  std::clog << "Wait for writter to join...\n";
  g_stop_writing = true;
  g_output_cv.notify_all();
  writer_thread.join();
#endif

  return 0;
}
