#ifndef SNACKIS_IMAP_HPP
#define SNACKIS_IMAP_HPP

#include <curl/curl.h>
#include <vector>

#include "snackis/core/error.hpp"
#include "snackis/core/str.hpp"

namespace snackis {
  struct ImapError: public Error {
    ImapError(const str &msg);
  };

  struct Ctx;

  struct Imap {
    Ctx &ctx;
    CURL *client;
    
    Imap(Ctx &ctx);
    virtual ~Imap();
  };
    
  void noop(struct Imap &imap);
  void fetch(struct Imap &imap, std::vector<str> &msgs);
}

#endif
