#include "snackis/ctx.hpp"
#include "snackis/net/worker.hpp"

namespace snackis {
namespace net {
  static void do_run(Worker *w) {
    w->run();
  }

  Worker::Worker(Ctx &ctx):
    ctx(ctx.proc, ctx.inbox.max),
    running(false) {
    this->ctx.secret = ctx.secret;
    db::copy(this->ctx.db.settings, ctx.db.settings);
    db::copy(this->ctx.db.peers, ctx.db.peers);
  }
  
  Worker::~Worker() {
    if (running) {
      running = false;
      go.notify_one();
      thread.join();
    }
  }

  void start(Worker &w) {
    CHECK(!w.running, _);
    w.running = true;
    w.thread = std::thread(do_run, &w);
  }
}}
