#ifndef SNACKIS_NET_WORKER_HPP
#define SNACKIS_NET_WORKER_HPP

#include <condition_variable>
#include <mutex>
#include <thread>

#include "snackis/ctx.hpp"

namespace snackis {
namespace net {
  struct Worker {
    using Lock = std::unique_lock<std::mutex>;

    Ctx ctx;
    std::mutex mutex;
    std::condition_variable go;
    std::thread thread;
    bool running;
    
    Worker(Ctx &ctx);
    virtual ~Worker();
    virtual void run()=0;
  };

  void start(Worker &w);
}}

#endif
