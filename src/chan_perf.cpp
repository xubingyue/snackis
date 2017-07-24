#include <vector>
#include <thread>
#include "snackis/core/chan.hpp"

using namespace snackis;

void run_pub(Chan<int> *ch, int reps) {
  for (int i(0); i < reps; i++) {
    put(*ch, i);
  }
}

void run_con(Chan<int> *ch, int reps) {
  for (int i(0); i < reps; i++) {
    get(*ch);
  }
}

void run(int workers, int reps, int buf) {
  std::vector<std::thread> wg;
  Chan<int> ch(buf);
  
  for (int i(0); i < workers; i++) {
    wg.emplace_back(run_pub, &ch, reps);
    wg.emplace_back(run_con, &ch, reps);
  }

  for (auto &t: wg) { t.join(); }
}

const int
  MAX_WORKERS(   5),
  MAX_REPS(1000000),
  MAX_BUF(    1000);

int main() {
  for (int workers(1); workers < MAX_WORKERS; workers++) {
    for(int reps(10); reps < MAX_REPS; reps *= 10) {
      for(int buf(1); buf < MAX_BUF; buf *= 10) {
	run(workers, reps, buf);
      }
    }
  }

  return 0;
}
