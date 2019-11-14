#include <iostream>
#include <array>
#include <vector>
#include <list>
#include <string>
#include <chrono>
#include <algorithm>
#include <random>

bool g_print_traces = true;

class TicToc {
  std::chrono::time_point<std::chrono::steady_clock> m_begin;

public:
  TicToc() : m_begin(std::chrono::high_resolution_clock::now()) {}
  ~TicToc() {
    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_begin).count();
    std::cout << duration / 1000.0 << "ms\n";
  }
};

class C {
  int m_id = -1;

public:
  C() {
    if (g_print_traces) { std::cout << " C::C() " << this << ' ' << m_id << '\n'; }
  }

  explicit C(int id) : m_id(id) {
    if (g_print_traces) { std::cout << " C::C(int) " << this << ' ' << m_id << '\n'; }
  }

  C(const C& other) : m_id(other.m_id) {
    if (g_print_traces) { std::cout << " C::C(const C&) " << this << ' ' << m_id << '\n'; }
  }

  ~C() {
    if (g_print_traces) { std::cout << " C::~C() " << this << ' ' << m_id << '\n'; }
  }

  bool operator<(const C& other) { return m_id < other.m_id; }
};

class D : public C {
  std::array<int, 4096> m_data;

public:
  using C::C;
};

template<typename T>
void print_vector_info(const std::vector<T>& a_vector)
{
  std::cout << "Size: " << a_vector.size() << '\n';
  std::cout << "Capacity: " << a_vector.capacity() << '\n';
  if (!a_vector.empty()) {
    std::cout << "Address of first element: " << &a_vector.front() << '\n';
    std::cout << "Address of last element: " << &a_vector.back() << '\n';
  }
}

template<typename T>
void print_list_info(const std::list<T>& a_list)
{
  std::cout << "Size: " << a_list.size() << '\n';
  if (!a_list.empty()) {
    std::cout << "Address of first element: " << &a_list.front() << '\n';
    std::cout << "Address of last element: " << &a_list.back() << '\n';
  }
}

void vector_push_back(int n, bool reserve)
{
  std::vector<C> c;

  std::cout << "\n\n\n" << __FUNCTION__ << ' ' << n << ' ' << reserve << '\n';
  if (reserve) {
    c.reserve(n);
  }
  for (int ii = 0; ii < n; ++ii) {
    c.push_back(C(ii));
    print_vector_info(c);
  }
}

void list_push_back(int n)
{
  std::list<C> c;

  std::cout << "\n\n\n" << __FUNCTION__ << ' ' << n << '\n';
  for (int ii = 0; ii < n; ++ii) {
    c.push_back(C(ii));
    print_list_info(c);
  }
}

void vector_push_back_ptr(int n)
{
  std::vector<C*> c;

  std::cout << "\n\n\n" << __FUNCTION__ << ' ' << n << '\n';
  for (int ii = 0; ii < n; ++ii) {
    c.push_back(new C(ii));
    print_vector_info(c);
  }
}

void sort_vector(int n)
{
  {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, n);
    std::vector<C> c;

    std::cout << "\n\n\n" << __FUNCTION__ << ' ' << n << '\n';
    g_print_traces = false; // temporary disable tracing
    for (int ii = 0; ii < n; ++ii) {
      c.push_back(C(dist(mt)));
    }
    g_print_traces = true;

    std::sort(c.begin(), c.end());

    g_print_traces = false;
  }
  g_print_traces = true;
}

void sort_list(int n)
{
  {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(0, n);
    std::list<C> c;

    std::cout << "\n\n\n" << __FUNCTION__ << ' ' << n << '\n';
    g_print_traces = false; // temporary disable tracing
    for (int ii = 0; ii < n; ++ii) {
      c.push_back(C(dist(mt)));
    }
    g_print_traces = true;

    c.sort();

    g_print_traces = false; // temporary disable tracing
  }
  g_print_traces = true;
}

template<class T>
void profile_push_backs(int n)
{
  g_print_traces = false; // temporary disable tracing
  {
    T c;

    std::cout << "\n\n\n" << __FUNCTION__ << ' ' << typeid(T).name() << ' ' << n << '\n';

    TicToc tic_toc;

    for (int ii = 0; ii < n; ++ii) {
      c.push_back(T::value_type(ii));
    }
  }
  g_print_traces = true;
}

template<class T>
void profile_access_to_ith_element(int n)
{
  g_print_traces = false; // temporary disable tracing
  {
    std::mt19937 mt(1000);
    std::uniform_int_distribution<int> dist(0, n - 1);
    T c;

    std::cout << "\n\n\n" << __FUNCTION__ << ' ' << typeid(T).name() << ' ' << n << '\n';
    for (int ii = 0; ii < n; ++ii) {
      c.push_back(T::value_type(ii));
    }

    TicToc tic_toc;
    for (int ii = 0; ii < n; ++ii) {
      auto it = c.begin();
      std::advance(it, dist(mt));
    }
  }
  g_print_traces = true;
}

int main()
{
  // Sample
  if (true) {
    C c(42);
  }

  // Question 1
  if (false) {
    vector_push_back(16, false);
    vector_push_back(16, true);
    list_push_back(16);
    vector_push_back_ptr(16);
  }

  // Question 2
  if (false) {
    sort_vector(16);
    sort_list(16);
  }

  // Question 3
  if (false) {
    if (false) {
      const int n = 100000;
      profile_push_backs<std::vector<C>>(n);
      profile_push_backs<std::list<C>>(n);
    }

    if (false) {
      const int n = 100000;
      profile_push_backs<std::vector<D>>(n);
      profile_push_backs<std::list<D>>(n);
    }
  }

  // Question 4
  if (false) {
    const int n = 10000;
    profile_access_to_ith_element<std::vector<D>>(n);
    profile_access_to_ith_element<std::list<D>>(n);
  }

  return 0;
}
