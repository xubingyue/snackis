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

  struct Imap {
    CURL *client;
    
    Imap(const str &url, int port,
	 const str &usr, const str &pwd);

    ~Imap();
  };
    
  void fetch(struct Imap &imap, std::vector<str> &msgs);
}

#endif
