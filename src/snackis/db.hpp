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
    db::Table<BasicSetting> settings;
    
    db::Table<Invite> invites;
	    
    db::Table<Peer> peers, peers_sort;

    db::Table<Feed> feeds, feeds_sort;
    db::Schema<Feed> feeds_share;

    db::Table<Post> posts, posts_sort, feed_posts;
    db::Schema<Post> posts_share;
    
    db::Table<Msg> inbox, inbox_sort, outbox;

    db::Table<Project> projects, projects_sort;
    db::Schema<Project> projects_share;

    db::Table<Task> tasks, tasks_sort;
    db::Schema<Task> tasks_share;

    Db(Ctx &ctx);
  };
}

#endif
