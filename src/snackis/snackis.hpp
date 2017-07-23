#ifndef SNACKIS_HPP
#define SNACKIS_HPP

#include "snackis/core/str.hpp"
#include "snackis/net/imap_worker.hpp"
#include "snackis/net/smtp_worker.hpp"

namespace snackis {
  extern const int VERSION[3];
  extern const int64_t DB_REV, PROTO_REV;

  extern opt<net::ImapWorker> imap_worker;
  extern opt<net::SmtpWorker> smtp_worker;

  void init();
  str version_str();
}

#endif
