#include "snackis/db/loop.hpp"

namespace snackis {
namespace db {
  static void run(Loop *lp) {
    while (true) {
      TRY(try_msg);
      auto msg(get(lp->inbox));
      if (!msg) { break; }
      lp->on_msg(*msg);
    }
  }
		   
  Loop::Loop(Proc &proc, size_t max_buf):
    proc(proc),
    inbox(max_buf)
  { }
  
  void start(Loop &lp) {
    lp.thread = std::thread(run, &lp);
  }

  void stop(Loop &lp) {
    close(lp.inbox);
    lp.thread.join();
  }
}}
