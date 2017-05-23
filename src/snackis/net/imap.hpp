#ifndef SNACKIS_IMAP_HPP
#define SNACKIS_IMAP_HPP

#include <curl/curl.h>
#include <vector>

#include "snackis/core/error.hpp"

namespace snackis {
  struct ImapError: public Error {
    ImapError(const std::string &msg);
  };

  struct Imap {
    CURL *client;
    
    Imap(const std::string &url, int port,
	 const std::string &usr, const std::string &pwd);

    ~Imap();
  };
    
  void fetch(struct Imap &imap, std::vector<std::string> &msgs);
}

#endif
