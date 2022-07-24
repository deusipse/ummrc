#include <iostream>
#include <thread>
#include <future>
#include <random>
#include <string>
#include <vector>

using namespace std;

random_device rd;
mt19937 rng(rd());

const char particles[] = "XY";

unsigned int num_threads = thread::hardware_concurrency();

string gen_tube(int length, double p) {
  discrete_distribution<int> pick{p, 1 - p};
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
    if (tube[i] == tube[i + 1] && tube[i] == 'X')
      return 0;
  }
  return 1;
}

double prob_no_two_consec_after_n(int n, int runs) {
  int count = 0;
  for (int i = 0; i < runs; i++) {
    if (check_consec_x(gen_tube(n, 0.5)))
      count++;
  }
  return double(count) / runs;
}

double average_len_after_n(int n, int runs) {
  long long count = 0;
  for (int i = 0; i < runs; i++) {
    count += annihilate(gen_tube(n, 0.5)).length();
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

double proportion_x(int runs, double p) {
  string tube = "";
  vector<future<string>> threads;
  for (int i = 0; i < num_threads; i++) {
    string small_tube = gen_tube(runs / num_threads, p);
    threads.push_back(async(launch::async, annihilate, small_tube));
  }
  for (auto &t : threads) {
    tube += t.get();
  }
  int len = tube.length();
  return (len - (1 - p) * runs) / len;
}

int main() {

  unsigned long long runs = 1000000;

  cout << "q1\n";
  for (int i = 1; i <= 10; i++) {
    cout << "n = " << i << " prob = " << prob_no_two_consec_after_n(i, runs) << '\n';
  }
  cout << "q2\n";
  for (int i = 1; i <= 10; i++) {
    cout << "n = " << i << " avg length = " << threaded_avg_len_after_n(i, runs) << '\n';
  }
  cout << "q3\n";
  for (double p = 0; p <= 10; p++) {
    cout << "p = " << p/10 << " proportion = " << proportion_x(runs, p/10) << '\n';
  }

  return 0;
}
