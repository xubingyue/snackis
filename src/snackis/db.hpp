#ifndef SNACKIS_DB_HPP
#define SNACKIS_DB_HPP

#include "snackis/feed.hpp"
#include "snackis/invite.hpp"
#include "snackis/msg.hpp"
#include "snackis/peer.hpp"
#include "snackis/post.hpp"
#include "snackis/project.hpp"
#include "snackis/setting.hpp"
#include "snackis/task.hpp"
#include "snackis/core/str.hpp"
#include "snackis/crypt/pub_key.hpp"
#include "snackis/db/col.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  struct Db {
    db::Table<BasicSetting, str> settings;
    
    db::Table<Invite, str> invites;
	    
    db::Table<Peer, UId> peers;
    db::Table<Peer, str, UId> peers_sort;

    db::Table<Feed, UId> feeds;
    db::Table<Feed, Time, UId> feeds_sort;
    db::Schema<Feed> feeds_share;

    db::Table<Post, UId> posts;
    db::Table<Post, Time, UId> posts_sort;
    db::Table<Post, UId, Time, UId> feed_posts;
    db::Schema<Post> posts_share;
    
    db::Table<Msg, UId> inbox, outbox;
    db::Table<Msg, Time, UId> inbox_sort;

    db::Table<Project, UId> projects;
    db::Table<Project, str, UId> projects_sort;
    db::Schema<Project> projects_share;

    db::Table<Task, UId> tasks;
    db::Table<Task, int64_t, Time, UId> tasks_sort;
    db::Schema<Task> tasks_share;

    Db(Ctx &ctx);
  };
}

#endif
