#include <chrono>
#include <future>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono;

std::random_device rd;
std::mt19937 rng(rd());

const char particles[] = "XY";

unsigned int num_threads = std::thread::hardware_concurrency();

std::string gen_tube(int length) {
  std::uniform_int_distribution<int> pick(0, 1);
  std::string tube;
  for (int i = 0; i < length; i++) {
    tube += particles[pick(rng)];
  }
  return tube;
}

std::string annihilate(std::string tube) {
  std::string res = "";
  for (char &c : tube) {
    if (res.size() && res.back() == c && c == 'X')
      res.pop_back();
    else
      res.push_back(c);
  }
  return res;
}

bool check_consec_x(std::string tube) {
  for (int i = 0; i < tube.length() - 1; i++) {
    if (tube[i] == tube[i + 1])
      return 0;
  }
  return 1;
}

double prob_consec(int length, int runs) {
  long count = 0;
  for (int i = 0; i < runs; i++) {
    if (check_consec_x(gen_tube(length)))
      count++;
  }
  return count/double(runs);
}

int main() {
  auto start = high_resolution_clock::now();

  int runs = 10000000;

  // std::vector<std::future<long>> threads;
  //
  // for (int i = 8; i < num_threads+8; i++) {
  //   threads.push_back(std::async(std::launch::async, count_check, i, runs));
  // }
  // for (auto &t : threads) {
  //   std::cout << t.get() << '\n';
  // }

  for (int i = 8; i < 16; i++) {
    std::cout << prob_consec(i, runs) << '\n';
  }

  auto stop = high_resolution_clock::now();
  auto elapsed = duration_cast<milliseconds>(stop - start);
  std::cout << elapsed.count() << "ms\n";
}
