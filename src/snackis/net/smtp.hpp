#ifndef SNACKIS_SMTP_HPP
#define SNACKIS_SMTP_HPP

#include <curl/curl.h>
#include <vector>

#include "snackis/core/data.hpp"
#include "snackis/core/error.hpp"
#include "snackis/db/trans.hpp"

namespace snackis {
  struct SmtpError: public Error {
    SmtpError(const str &msg);
  };

  struct Ctx;
  struct Msg;
  
  struct Smtp {
    Ctx &ctx;
    db::Trans trans;
    CURL *client;
    Data data;
    
    Smtp(Ctx &ctx);
    virtual ~Smtp();
  };
    
  void noop(const struct Smtp &smtp);
  void send(struct Smtp &smtp, Msg &msg);
  void send(struct Smtp &smtp);
}

#endif
