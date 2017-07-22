#include "snackis/db/loop.hpp"

namespace snackis {
namespace db {
  static void run(Loop *loop) {
    while (true) {
      TRY(try_msg);
      auto msg(get(loop->inbox));
      if (!msg) { break; }
      loop->on_msg(*msg);
    }
  }
		   
  Loop::Loop(Proc &proc, size_t max_buf):
    proc(proc),
    inbox(max_buf),
    thread(run, this)
  { }

  Loop::~Loop() {
    close(inbox);
    thread.join();
  }
}}
