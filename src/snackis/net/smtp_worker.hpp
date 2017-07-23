#ifndef SNACKIS_SMTP_WORKER_HPP
#define SNACKIS_SMTP_WORKER_HPP

#include <condition_variable>
#include <mutex>
#include <thread>

#include "snackis/ctx.hpp"

namespace snackis {
namespace net {
  struct SmtpWorker {
    using Lock = std::unique_lock<std::mutex>;

    Ctx ctx;
    std::mutex mutex;
    std::condition_variable go;
    std::thread thread;
    bool stopped;
    
    SmtpWorker(Ctx &ctx);
    ~SmtpWorker();
  };
}}

#endif
