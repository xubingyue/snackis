#ifndef SNACKIS_SMTP_WORKER_HPP
#define SNACKIS_SMTP_WORKER_HPP

#include <condition_variable>
#include <mutex>
#include <thread>

#include "snackis/net/worker.hpp"

namespace snackis {
namespace net {
  struct SmtpWorker: Worker {
    SmtpWorker(Ctx &ctx);
    void run() override;
  };
}}

#endif
