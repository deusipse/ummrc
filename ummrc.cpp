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
  for (int i = 0; i < tube.length() - 1; i++) {
    if (tube[i] != tube[i + 1] && tube[i] == 'X')
      output += tube[i];
    else if (tube[i] != 'X')
      output += tube[i];
  }
  output.push_back(tube.back());
  return output;
}

bool check_consec_x(string tube) {
  for (int i = 0; i < tube.length() - 1; i++) {
    if (tube[i] == tube[i + 1])
      return 0;
  }
  return 1;
}

double prob_no_consec_after_n(int n, int runs) { // Q1
  int count = 0;
  for (int i = 0; i < runs; i++) {
    if (check_consec_x(gen_tube(n)))
      count++;
  }
  return double(count) / runs;
}

double average_len_after_n(int n, int runs) { // Q2
  long long count = 0;
  for (int i = 0; i < runs; i++) {
    count += annihilate(gen_tube(n)).length();
  }
  return count / double(runs);
}

double threaded_avg_len_after_n(int n, int runs) {
  double avg = 0;
  vector<future<double>> threads;
  for (int i = 0; i < num_threads; i++) {
    threads.push_back(
        async(launch::async, average_len_after_n, n, runs / num_threads));
  }
  for (auto &t : threads) {
    avg += t.get();
  }
  return avg / num_threads;
}

double proportion_x(int n) {
  string tube = "";
  vector<future<string>> threads;
  for (int i = 0; i < num_threads; i++) {
    string small_tube = gen_tube(n / num_threads);
    threads.push_back(async(launch::async, annihilate, small_tube));
  }
  for (auto &t : threads) {
    tube += t.get();
  }
  int len = tube.length();
  return (len - 0.5 * n) / len;
}

int main() {
  auto start = high_resolution_clock::now();

  vector<future<double>> threads;
  unsigned long long runs = 10000000;

  // double avg = 0;
  // for (int i = 0; i < num_threads; i++) {
  //   threads.push_back(async(launch::async, average_len_after_n, 10,
  //   runs/num_threads));
  // }
  // for (auto &t : threads) {
  //   avg += t.get();
  // }
  // for (int i = 1; i <= 11; i++) {
  //   cout << threaded_avg_len_after_n(i, runs) << '\n';
  // }

  // int count = 0;
  // string tube = annihilate(gen_tube(100000));
  // for (char &c : tube) {
  //   if (c == 'Y')
  //     count++;
  // }

  cout << proportion_x(runs) << '\n';

  auto stop = high_resolution_clock::now();
  auto elapsed = duration_cast<milliseconds>(stop - start);
  cout << elapsed.count() << "ms\n";
}
