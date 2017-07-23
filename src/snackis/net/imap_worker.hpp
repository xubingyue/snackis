#ifndef SNACKIS_IMAP_WORKER_HPP
#define SNACKIS_IMAP_WORKER_HPP

#include <condition_variable>
#include <mutex>
#include <thread>

#include "snackis/net/worker.hpp"

namespace snackis {
namespace net {
  struct ImapWorker: Worker {
    ImapWorker(Ctx &ctx);
    void run() override;
  };
}}

#endif
