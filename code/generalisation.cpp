#include <future>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

using namespace std;
using namespace std::chrono;

random_device rd;

const char particles[] = "XY";
unsigned int num_threads = thread::hardware_concurrency();

using namespace std;

string gen_tube(int length, double p) {
  mt19937 rng(rd());
  discrete_distribution<int> pick{p, 1 - p};
  string tube;
  for (int i = 0; i < length; i++) {
    tube += particles[pick(rng)];
  }
  return tube;
}

string binary_to_tube(string s) {
  string out;
  for (char c : s)
    (c == '1') ? (out += 'X') : (out += 'Y');
  return out;
}

string annihilate(string A, int m, int n) {
  long long cnt = 0;
  string B;
  for (char x : A) {
    long long k = cnt;
    while (k >= m)
      k = (k / m) * n + (k % m);
    x == 'X' ? (cnt++) : (B += string(k, 'X') + "Y", cnt = 0);
  }
  while (cnt >= m)
    cnt = (cnt / m) * n + (cnt % m);
  B += string(cnt, 'X');
  return B;
}

long double average_len_after_n(int k, int runs, int m, int n, double p) {
  unsigned long long count = 0;
  for (int i = 0; i < runs; i++) {
    count += annihilate(gen_tube(k, p), m, n).length();
  }
  return count / double(runs);
}

long double brute_force_avg(int k, int m, int n) {
  long long cnt = 0;
  for (int i = 0; i < (1 << k); i++) {
    string tube;
    for (int j = k - 1; j >= 0; j--) {
      tube += to_string((i >> j) & 1);
    }
    cnt += annihilate(binary_to_tube(tube), m, n).length();
  }
  return (long double)(cnt) / (1 << k);
}

long double threaded_avg_len_after_n(int k, int runs, int m, int n, double p) {
  long double avg = 0;
  vector<future<long double>> threads;
  for (int i = 0; i < num_threads; i++) {
    threads.push_back(async(launch::async, average_len_after_n, k,
                            runs / num_threads, m, n, p));
  }
  for (auto &t : threads) {
    avg += t.get();
  }
  return avg / num_threads;
}

int main() {

  long long runs = 1000000;

  int m = 6;
  int n = 5;

  for (int i = 1; i <= 15; i++) {
    cout << "n = " << i << " m = " << m << " n = " << n
         << " p = 0.5 avg = " << brute_force_avg(i, m, n) << '\n';
  }

  m = 4;
  n = 1;

  double p = 0.42;
  for (int i = 1; i <= 15; i++) {
    cout << "n = " << i << " m = " << m << " n = " << n << " p = " << p
         << " avg = " << threaded_avg_len_after_n(i, runs, m, n, p) << '\n';
  }

  return 0;
}
