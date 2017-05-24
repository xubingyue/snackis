#include <iostream>
#include <iterator>
#include "snackis/core/buf.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/net/imap.hpp"

namespace snackis {
  ImapError::ImapError(const str &msg):
    Error(str("ImapError: ") + msg) { }

  static size_t on_write(char *ptr, size_t size, size_t nmemb, void *_out) {
    Buf *out = static_cast<Buf *>(_out);
    out->write(ptr, size * nmemb);
    return size * nmemb;  
  }

  static size_t skip_write(char *ptr, size_t size, size_t nmemb, void *_out) {
    return size * nmemb;  
  }

  Imap::Imap(const str &url, int port,
	     const str &usr, const str &pwd):
    client(curl_easy_init()) {
    if (client) {
      curl_easy_setopt(client, CURLOPT_USERNAME, usr.c_str());
      curl_easy_setopt(client, CURLOPT_PASSWORD, pwd.c_str());
      curl_easy_setopt(client, CURLOPT_URL,
		       fmt("imaps://%1%:%2%/INBOX") % url % port);
      curl_easy_setopt(client, CURLOPT_WRITEFUNCTION, on_write);
      //curl_easy_setopt(client, CURLOPT_VERBOSE, 1L);
    }
  }

  Imap::~Imap() {
    curl_easy_cleanup(client);
  }

  static void delete_uid(struct Imap &imap, const str &uid) {
    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     fmt("UID STORE %1% +FLAGS.SILENT \\Deleted") % uid);

    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, skip_write);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed deleting uid: %1%") % curl_easy_strerror(res)); 
    }
  }

  static void expunge(struct Imap &imap) {
    curl_easy_setopt(imap.client, CURLOPT_CUSTOMREQUEST, "EXPUNGE");

    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, skip_write);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed expunging: %1%") % curl_easy_strerror(res));
    }
  }

  static str fetch_uid(struct Imap &imap, const str &uid) {
    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     fmt("UID FETCH %1% BODY[TEXT]") % uid);

    Buf out;    
    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, on_write);
    curl_easy_setopt(imap.client, CURLOPT_HEADERDATA, &out);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed fetching uid: %1%") % curl_easy_strerror(res));
    }

    std::vector<str> lines;
    str line;
    
    while (std::getline(out, line, '\n')) {
      lines.push_back(line);
    }
      
    out.str("");
    out.clear();
    
    for (auto i = std::next(lines.begin(), 1);
	 i != std::next(lines.begin(), lines.size()-2);
	 i++) {
      out << *i << std::endl;
    }
    
    return out.str();
  }
  
  void fetch(struct Imap &imap, std::vector<str> &msgs) {
    if (!imap.client) {
      ERROR(Imap, fmt("Failed connecing"));
    }

    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     "UID SEARCH Subject \"__SNACKIS__\"");

    Buf out;    
    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_HEADERDATA, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_WRITEDATA, &out);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed fetching: %1%") % curl_easy_strerror(res));
    }

    std::vector<str> tokens{
      std::istream_iterator<str>{out},
	std::istream_iterator<str>{}};

    if (tokens.size() < 2 || tokens[1] != "SEARCH") {
      ERROR(Imap, fmt("Invalid fetch result:\n%1%") % out.str()); 
    }
    
    for (auto tok = std::next(tokens.begin(), 2); tok != tokens.end(); tok++) {
      const str uid(*tok);
      msgs.push_back(fetch_uid(imap, uid));
      delete_uid(imap, uid);
    }

    if (tokens.size() > 2) {
      expunge(imap);
    }
  }
}
