#include <iostream>
#include <iterator>
#include "snackis/ctx.hpp"
#include "snackis/core/buf.hpp"
#include "snackis/core/format.hpp"
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

  Imap::Imap(Ctx &ctx): ctx(ctx), trans(ctx), client(curl_easy_init()) {
    if (!client) { ERROR(Imap, "Failed initializing client"); }
    curl_easy_setopt(client, 
		     CURLOPT_USERNAME, 
		     get_val(ctx.settings.imap_user)->c_str());
    curl_easy_setopt(client, 
		     CURLOPT_PASSWORD, 
		     get_val(ctx.settings.imap_pass)->c_str());
    curl_easy_setopt(client,
		     CURLOPT_URL,
		     format("imaps://{0}:{1}/INBOX",
			    *get_val(ctx.settings.imap_url),
			    *get_val(ctx.settings.imap_port)).c_str());
    curl_easy_setopt(client, CURLOPT_WRITEFUNCTION, on_write);
    //curl_easy_setopt(client, CURLOPT_VERBOSE, 1L);
    
    log(ctx, "Connecting to Imap");

    try {
      noop(*this);
    } catch (const ImapError &e) {
      ERROR(Imap, format("Failed connecting: {0}", e.what()));
    }
  }

  Imap::~Imap() { curl_easy_cleanup(client); }

  void noop(const struct Imap &imap) {
    curl_easy_setopt(imap.client, CURLOPT_CUSTOMREQUEST, "NOOP");
    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, skip_write);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, format("Failed sending NOOP: {0}", curl_easy_strerror(res))); 
    }
  }

  static void delete_uid(const struct Imap &imap, const str &uid) {
    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     format("UID STORE {0} +FLAGS.SILENT \\Deleted", uid).c_str());

    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, skip_write);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, format("Failed deleting uid: {0}", curl_easy_strerror(res))); 
    }
  }

  static void expunge(const struct Imap &imap) {
    curl_easy_setopt(imap.client, CURLOPT_CUSTOMREQUEST, "EXPUNGE");

    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, skip_write);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, format("Failed expunging: {0}", curl_easy_strerror(res)));
    }
  }

  static str fetch_uid(const struct Imap &imap, const str &uid) {
    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     format("UID FETCH {0} BODY[TEXT]", uid).c_str());

    Buf out;    
    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, on_write);
    curl_easy_setopt(imap.client, CURLOPT_HEADERDATA, &out);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, format("Failed fetching uid: {0}", curl_easy_strerror(res)));
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
  
  void fetch(struct Imap &imap) {
    log(imap.ctx, "Fetching email");
    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     "UID SEARCH Subject \"__SNACKIS__\"");

    Buf out;    
    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_HEADERDATA, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, on_write);
    curl_easy_setopt(imap.client, CURLOPT_WRITEDATA, &out);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, format("Failed fetching: {0}", curl_easy_strerror(res)));
    }

    std::vector<str> tokens{
      std::istream_iterator<str>{out},
	std::istream_iterator<str>{}};

    if (tokens.size() < 2 || tokens[1] != "SEARCH") {
      ERROR(Imap, format("Invalid fetch result:\n{0}", out.str())); 
    }
    
    for (auto tok = std::next(tokens.begin(), 2); tok != tokens.end(); tok++) {
      const str uid(*tok);
      auto msg = fetch_uid(imap, uid);
      delete_uid(imap, uid);
    }

    if (tokens.size() > 2) {
      expunge(imap);
    }

    db::commit(imap.trans);
    log(imap.ctx, "OK");
  }
}
