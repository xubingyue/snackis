#ifndef SNACKIS_IMAP_WORKER_HPP
#define SNACKIS_IMAP_WORKER_HPP

#include <condition_variable>
#include <mutex>
#include <thread>

#include "snackis/ctx.hpp"

namespace snackis {
namespace net {
  struct ImapWorker {
    using Lock = std::unique_lock<std::mutex>;

    Ctx ctx;
    std::mutex mutex;
    std::condition_variable go;
    std::thread thread;
    bool stopped;
    
    ImapWorker(Ctx &ctx);
    ~ImapWorker();
  };
}}

#endif
