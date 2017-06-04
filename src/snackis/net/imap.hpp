#ifndef SNACKIS_IMAP_HPP
#define SNACKIS_IMAP_HPP

#include <curl/curl.h>
#include <vector>

#include "snackis/core/error.hpp"
#include "snackis/core/str.hpp"
#include "snackis/db/trans.hpp"

namespace snackis {
  struct ImapError: public Error {
    ImapError(const str &msg);
  };

  struct Ctx;

  struct Imap {
    Ctx &ctx;
    db::Trans trans;
    CURL *client;
    
    Imap(Ctx &ctx);
    virtual ~Imap();
  };
    
  void noop(const struct Imap &imap);
  void fetch(struct Imap &imap);
}

#endif
