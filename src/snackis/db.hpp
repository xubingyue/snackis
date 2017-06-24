#ifndef SNACKIS_DB_HPP
#define SNACKIS_DB_HPP

#include "snackis/invite.hpp"
#include "snackis/msg.hpp"
#include "snackis/peer.hpp"
#include "snackis/post.hpp"
#include "snackis/project.hpp"
#include "snackis/setting.hpp"
#include "snackis/feed.hpp"
#include "snackis/core/str.hpp"
#include "snackis/crypt/pub_key.hpp"
#include "snackis/db/col.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  struct Db {
    db::Col<BasicSetting, str> setting_key, setting_val;
    db::Table<BasicSetting> settings;
    
    db::Col<Invite, str>  invite_to;
    db::Col<Invite, Time> invite_posted_at;
    db::Table<Invite> invites;
	    
    db::Col<Peer, UId>           peer_id;
    db::Col<Peer, str>           peer_name;
    db::Col<Peer, str>           peer_email;
    db::Col<Peer, crypt::PubKey> peer_crypt_key;
    db::Table<Peer> peers;

    db::Col<Feed, UId>           feed_id;
    db::Col<Feed, str>           feed_name;
    db::Col<Feed, bool>          feed_active;
    db::Col<Feed, std::set<UId>> feed_peer_ids;
    db::Table<Feed> feeds;

    db::Col<Post, UId>           post_id;
    db::Col<Post, UId>           post_feed_id;
    db::Col<Post, Time>          post_at;
    db::Col<Post, UId>           post_by_id;
    db::Col<Post, str>           post_body;
    db::Table<Post> posts, feed_posts, at_posts;
    
    db::Col<Msg, UId>           msg_id;
    db::Col<Msg, str>           msg_type;
    db::Col<Msg, str>           msg_from, msg_to;
    db::Col<Msg, UId>           msg_from_id, msg_to_id;
    db::Col<Msg, Time>          msg_fetched_at;
    db::Col<Msg, str>           msg_peer_name;
    db::Col<Msg, crypt::PubKey> msg_crypt_key;
    db::Col<Msg, UId>           msg_feed_id;
    db::Col<Msg, str>           msg_feed_name;
    db::Col<Msg, UId>           msg_post_id;
    db::Col<Msg, Time>          msg_post_at;
    db::Col<Msg, str>           msg_post_body;
    db::Table<Msg> msgs, inbox, outbox;

    db::Col<Project, UId>           project_id;
    db::Col<Project, str>           project_name;
    db::Col<Project, UId>           project_owner_id;
    db::Col<Project, str>           project_info;
    db::Col<Project, std::set<UId>> project_peer_ids;
    db::Table<Project> projects;
    
    Db(Ctx &ctx);
  };
}

#endif
