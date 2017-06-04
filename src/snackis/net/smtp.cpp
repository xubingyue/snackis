#include <iostream>
#include <iterator>
#include "snackis/ctx.hpp"
#include "snackis/core/buf.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/net/smtp.hpp"

namespace snackis {
  SmtpError::SmtpError(const str &msg): Error(str("SmtpError: ") + msg) { }

  static size_t on_read(char *ptr, size_t size, size_t nmemb, void *_out) {
    Buf *out = static_cast<Buf *>(_out);
    out->write(ptr, size * nmemb);
    return size * nmemb;  
  }

  static size_t on_write(void *ptr, size_t size, size_t nmemb, void *_smtp) {
    if (!_smtp || !size || !nmemb || ((size*nmemb) < 1)) {
      return 0;
    }
  
    Smtp *smtp = static_cast<Smtp *>(_smtp);

    const size_t len(smtp->data.size());
    memcpy(ptr, &smtp->data[0], len);
    curl_easy_setopt(smtp->client, CURLOPT_READDATA, nullptr);
    return len;
  }
  
  Smtp::Smtp(Ctx &ctx): ctx(ctx), trans(ctx), client(curl_easy_init()) {
    if (!client) { ERROR(Smtp, "Failed initializing client"); }
    curl_easy_setopt(client, 
		     CURLOPT_USERNAME, 
		     get_val(ctx.settings.smtp_user)->c_str());
    curl_easy_setopt(client, 
		     CURLOPT_PASSWORD, 
		     get_val(ctx.settings.smtp_pass)->c_str());
    curl_easy_setopt(client,
		     CURLOPT_URL,
		     fmt("smtp://%0:%1",
			 *get_val(ctx.settings.smtp_url),
			 *get_val(ctx.settings.smtp_port)).c_str());
    curl_easy_setopt(client, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    curl_easy_setopt(client, CURLOPT_READFUNCTION, on_write);
    curl_easy_setopt(client, CURLOPT_READDATA, this);
    curl_easy_setopt(client, CURLOPT_WRITEFUNCTION, on_read);
    //curl_easy_setopt(client, CURLOPT_VERBOSE, 1L);
    
    log(ctx, "Connecting to Smtp");

    try {
      noop(*this);
    } catch (const SmtpError &e) {
      ERROR(Smtp, fmt("Failed connecting: %0", e.what()));
    }
  }

  Smtp::~Smtp() { curl_easy_cleanup(client); }

  void noop(const struct Smtp &smtp) {
    curl_easy_setopt(smtp.client, CURLOPT_CUSTOMREQUEST, "NOOP");
    curl_easy_setopt(smtp.client, CURLOPT_UPLOAD, 0L);

    Buf resp_buf;
    curl_easy_setopt(smtp.client, CURLOPT_WRITEDATA, &resp_buf);
    CURLcode res(curl_easy_perform(smtp.client));
 
    if (res != CURLE_OK) {
      ERROR(Smtp, fmt("Failed sending NOOP: %0", curl_easy_strerror(res))); 
    }

    std::vector<str> resp {
      std::istream_iterator<str>{resp_buf}, std::istream_iterator<str>{}
    };

    if (resp.size() < 3 || resp[2] != "OK") {
      ERROR(Smtp, fmt("Invalid NOOP response: %0", resp_buf.str()));
    }
  }

  void send(struct Smtp &smtp) {
    log(smtp.ctx, "Sending email");
    curl_easy_setopt(smtp.client, CURLOPT_UPLOAD, 1L);

    Buf resp_buf;
    curl_easy_setopt(smtp.client, CURLOPT_WRITEDATA, &resp_buf);

    db::Table<Msg> &tbl(smtp.ctx.db.outbox);
    while (tbl.recs.size() > 0) {
      Msg msg(tbl, *tbl.recs.begin());
      curl_easy_setopt(smtp.client, CURLOPT_MAIL_FROM, msg.peer_email.c_str());
      
      load(smtp.ctx.db.peers, msg.sent_to);
      struct curl_slist *sent_to = nullptr;
      sent_to = curl_slist_append(sent_to,
				  get<str>(msg.sent_to.at(&smtp.ctx.db.peer_email)).
				  c_str());
      curl_easy_setopt(smtp.client, CURLOPT_MAIL_RCPT, sent_to);
      
      smtp.data.clear();
      //TODO: encode msg to smtp.data
      resp_buf.str("");
      CURLcode res(curl_easy_perform(smtp.client));
      
      if (res != CURLE_OK) {
	ERROR(Smtp, fmt("Failed sending email: %0", curl_easy_strerror(res)));
      }

      std::vector<str> resp {
	std::istream_iterator<str>{resp_buf}, std::istream_iterator<str>{}
      };

      if (resp.size() < 3 || resp[2] != "OK") {
	ERROR(Smtp, fmt("Invalid send response: %0", resp_buf.str()));
      }

      tbl.recs.erase(tbl.recs.begin());
    }
    
    db::commit(smtp.trans);
    log(smtp.ctx, "OK");
  }
}
