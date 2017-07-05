#include <iostream>
#include <iterator>
#include "snackis/ctx.hpp"
#include "snackis/invite.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/core/stream.hpp"
#include "snackis/net/imap.hpp"

namespace snackis {
  ImapError::ImapError(const str &msg): Error(str("ImapError: ") + msg) { }

  static size_t on_read(char *ptr, size_t size, size_t nmemb, void *_out) {
    Stream *out = static_cast<Stream *>(_out);
    out->write(ptr, size * nmemb);
    return size * nmemb;  
  }

  static size_t skip_read(char *ptr, size_t size, size_t nmemb, void *_out) {
    return size * nmemb;  
  }

  Imap::Imap(Ctx &ctx): ctx(ctx), client(curl_easy_init()) {
    if (!client) { ERROR(Imap, "Failed initializing client"); }
    curl_easy_setopt(client, 
		     CURLOPT_USERNAME, 
		     get_val(ctx.settings.imap_user)->c_str());
    curl_easy_setopt(client, 
		     CURLOPT_PASSWORD, 
		     get_val(ctx.settings.imap_pass)->c_str());
    curl_easy_setopt(client,
		     CURLOPT_URL,
		     fmt("imaps://%0:%1/INBOX",
			 *get_val(ctx.settings.imap_url),
			 *get_val(ctx.settings.imap_port)).c_str());
    curl_easy_setopt(client, CURLOPT_WRITEFUNCTION, on_read);
    //curl_easy_setopt(client, CURLOPT_VERBOSE, 1L);
    
    log(ctx, "Connecting to Imap...");

    try {
      noop(*this);
    } catch (const ImapError &e) {
      ERROR(Imap, fmt("Failed connecting: %0", e.what()));
    }
  }

  Imap::~Imap() { curl_easy_cleanup(client); }

  void noop(const struct Imap &imap) {
    curl_easy_setopt(imap.client, CURLOPT_CUSTOMREQUEST, "NOOP");
    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, skip_read);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed sending NOOP: %0", curl_easy_strerror(res))); 
    }
  }

  static void delete_uid(const struct Imap &imap, const str &uid) {
    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     fmt("UID STORE %0 +FLAGS.SILENT \\Deleted", uid).c_str());

    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, skip_read);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed deleting uid: %0", curl_easy_strerror(res))); 
    }
  }

  static void expunge(const struct Imap &imap) {
    curl_easy_setopt(imap.client, CURLOPT_CUSTOMREQUEST, "EXPUNGE");

    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, skip_read);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed expunging: %0", curl_easy_strerror(res)));
    }
  }

  static opt<Msg> fetch_uid(const struct Imap &imap, const str &uid) {
    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     fmt("UID FETCH %0 BODY[TEXT]", uid).c_str());

    Stream out;    
    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, on_read);
    curl_easy_setopt(imap.client, CURLOPT_HEADERDATA, &out);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed fetching uid: %0", curl_easy_strerror(res)));
    }

    db::Rec<Msg> rec;
    Msg msg(imap.ctx, rec);
    const str body(out.str());
    const str tag("__SNACKIS__\r\n");
    auto i(body.find(tag) + tag.size());
    if (i == str::npos || !decode(msg, body.substr(i))) { return nullopt; }
    return msg;
  }

  static void handle_msg(Imap &imap, const Msg &msg) {
    Ctx &ctx(imap.ctx);

    if (msg.type == Msg::ACCEPT || msg.type == Msg::REJECT) {
      db::Rec<Invite> inv;
      set(inv, ctx.db.invite_to, msg.from);
      
      if (!load(ctx.db.invites, inv)) {
	log(ctx, fmt("Missing invite: %0", msg.from));
	return;
      }
    }
    
    if (msg.type == Msg::REJECT) {
      invite_rejected(msg);
      log(ctx, fmt("Invite to %0 was rejected", msg.from));
    } else if (msg.type == Msg::POST) {
      opt<Feed> feed_found(find_feed_id(ctx,
					*db::get(msg.feed, ctx.db.feed_id)));
      if (feed_found) {
	opt<Post> post_found(find_post_id(ctx,
					  *db::get(msg.post, ctx.db.post_id)));
	Peer peer(get_peer_id(ctx, msg.from_id));
	
	if (post_found) {
	  if (peer.id == post_found->owner_id) {
	    copy(*feed_found, msg);
	    db::update(ctx.db.feeds, *feed_found);
	    
	    copy(*post_found, msg);
	    db::update(ctx.db.posts, *post_found);
	    
	    log(ctx, fmt("Post updated in feed '%0' by %1:\n%2",
			 id_str(*feed_found),
			 peer.name,
			 post_found->body));
	  } else {
	    log(ctx, fmt("Skipping unautorized post update from %0",
			 msg.from));
	  }
	} else {
	  copy(*feed_found, msg);
	  db::update(ctx.db.feeds, *feed_found);
	  
	  Post post(msg);
	  db::insert(ctx.db.posts, post);
	  
	  log(ctx, fmt("New post in feed '%0' by %1:\n%2",
		       id_str(*feed_found),
		       peer.name,
		       post.body));
	}
      } else {
	db::insert(ctx.db.inbox, msg);
      }
    } else {
      db::insert(ctx.db.inbox, msg);
    }
  }
  
  void fetch(struct Imap &imap) {
    TRACE("Fetching email");
    Ctx &ctx(imap.ctx);
    
    log(ctx, "Fetching email...");
    curl_easy_setopt(imap.client,
		     CURLOPT_CUSTOMREQUEST,
		     "UID SEARCH Subject \"__SNACKIS__\"");

    Stream out;    
    curl_easy_setopt(imap.client, CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_HEADERDATA, nullptr);
    curl_easy_setopt(imap.client, CURLOPT_WRITEFUNCTION, on_read);
    curl_easy_setopt(imap.client, CURLOPT_WRITEDATA, &out);
    CURLcode res(curl_easy_perform(imap.client));
 
    if (res != CURLE_OK) {
      ERROR(Imap, fmt("Failed searching inbox: %0", curl_easy_strerror(res)));
    }

    std::vector<str> tokens {
      std::istream_iterator<str>{out}, std::istream_iterator<str>{}
    };

    if (tokens.size() < 2 || tokens[1] != "SEARCH") {
      ERROR(Imap, fmt("Invalid fetch result:\n%0", out.str())); 
    }

    int msg_cnt = 0;
    
    for (auto tok = std::next(tokens.begin(), 2); tok != tokens.end(); tok++) {
      db::Trans trans(ctx);
      
      try {
	const str uid(*tok);
	opt<Msg> msg = fetch_uid(imap, uid);
	
	if (msg) {
	  handle_msg(imap, *msg);
	  msg_cnt++;
	} else {
	  log(ctx, "Failed decoding message");
	}

	delete_uid(imap, uid);
	db::commit(trans);
      } catch (const std::exception &e) {
	log(ctx, fmt("Error while processing message: %0", e.what()));
      }
    }

    if (tokens.size() > 2) {
      expunge(imap);
    }

    log(ctx, fmt("Finished fetching %0 messages", msg_cnt));
  }
}
