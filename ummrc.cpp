#include <chrono>
#include <future>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono;
using namespace std;

random_device rd;
mt19937 rng(rd());

const char particles[] = "XY";

unsigned int num_threads = thread::hardware_concurrency();

string gen_tube(int length) {
  uniform_int_distribution<int> pick(0, 1);
  string tube;
  for (int i = 0; i < length; i++) {
    tube += particles[pick(rng)];
  }
  return tube;
}

string annihilate(string tube) {
  string output = "";
  for (char &c : tube) {
    if (output.size() && output.back() == c && c == 'X')
      output.pop_back();
    else
      output.push_back(c);
  }
  return output;
}

bool check_consec_x(string tube) {
  for (int i = 0; i < tube.length() - 1; i++) {
    if (tube[i] == tube[i + 1])
      return 0;
  }
  return 1;
}

int main() {
  auto start = high_resolution_clock::now();

  vector<future<string>> threads;
  unsigned int len_tube = 1000000000;

  string tube;
  for (int i = 0; i < num_threads; i++) {                             // Multithreaded large annihilated tube
    string small_tube = gen_tube(len_tube/num_threads);
    threads.push_back(async(launch::async, annihilate, small_tube));
  }
  for (auto &t : threads) {
    tube += t.get();
  }

  cout << tube.length() << '\n';

  auto stop = high_resolution_clock::now();
  auto elapsed = duration_cast<milliseconds>(stop - start);
  cout << elapsed.count() << "ms\n";
}
