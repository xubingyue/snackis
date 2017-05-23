#include <iostream>
#include <iterator>
#include <sstream>
#include "snackis/net/imap.hpp"

namespace snackis {
  ImapError::ImapError(const std::string &msg):
    Error(std::string("ImapError: ") + msg) { }

  static size_t on_write(char *ptr, size_t size, size_t nmemb, void *_out) {
    std::stringstream *out = static_cast<std::stringstream *>(_out);
    out->write(ptr, size * nmemb);
    return size * nmemb;  
  }

  static size_t skip_write(char *ptr, size_t size, size_t nmemb, void *_out) {
    return size * nmemb;  
  }

  Imap::Imap(const std::string &url, int port,
	     const std::string &usr, const std::string &pwd):
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

  static void delete_uid(struct Imap &imap, const std::string &uid) {
    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     fmt("UID STORE %1% +FLAGS.SILENT \\Deleted") % uid);

    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, NULL);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, skip_write);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed deleting uid: %1%") % curl_easy_strerror(res)); 
    }
  }

  static void expunge(struct Imap &imap) {
    curl_easy_setopt(imap.client, CURLOPT_CUSTOMREQUEST, "EXPUNGE");

    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, NULL);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, skip_write);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed expunging: %1%") % curl_easy_strerror(res));
    }
  }

  static std::string fetch_uid(struct Imap &imap, const std::string &uid) {
    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     fmt("UID FETCH %1% BODY[TEXT]") % uid);

    std::stringstream out;    
    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, on_write);
    curl_easy_setopt(imap.client, CURLOPT_HEADERDATA, &out);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed fetching uid: %1%") % curl_easy_strerror(res));
    }

    std::vector<std::string> lines;
    std::string line;
    
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
  
  void fetch(struct Imap &imap, std::vector<std::string> &msgs) {
    if (!imap.client) {
      ERROR(Imap, fmt("Failed connecing"));
    }

    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     "UID SEARCH Subject \"__SNACKIS__\"");

    std::stringstream out;    
    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, NULL);
    curl_easy_setopt(imap.client, CURLOPT_HEADERDATA, NULL);
    curl_easy_setopt(imap.client, CURLOPT_WRITEDATA, &out);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed fetching: %1%") % curl_easy_strerror(res));
    }

    std::vector<std::string> tokens{
      std::istream_iterator<std::string>{out},
	std::istream_iterator<std::string>{}};

    if (tokens.size() < 2 || tokens[1] != "SEARCH") {
      ERROR(Imap, fmt("Invalid fetch result:\n%1%") % out.str()); 
    }
    
    for (auto tok = std::next(tokens.begin(), 2); tok != tokens.end(); tok++) {
      const std::string uid(*tok);
      msgs.push_back(fetch_uid(imap, uid));
      delete_uid(imap, uid);
    }

    if (tokens.size() > 2) {
      expunge(imap);
    }
  }
}
