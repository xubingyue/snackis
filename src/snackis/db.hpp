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
    db::Col<BasicSetting, str> setting_key, setting_val;
    db::Table<BasicSetting> settings;
    
    db::Col<Invite, str>  invite_to;
    db::Col<Invite, Time> invite_posted_at;
    db::Table<Invite> invites;
	    
    db::Col<Peer, UId>           peer_id;
    db::Col<Peer, Time>          peer_created_at, peer_changed_at;
    db::Col<Peer, str>           peer_name, peer_email;
    db::Col<Peer, std::set<str>> peer_tags;
    db::Col<Peer, crypt::PubKey> peer_crypt_key;
    db::Col<Peer, bool>          peer_active;
    db::Table<Peer> peers, peers_sort;

    db::Col<Feed, UId>           feed_id;
    db::Col<Feed, UId>           feed_owner_id;
    db::Col<Feed, Time>          feed_created_at, feed_changed_at;
    db::Col<Feed, str>           feed_name, feed_info;
    db::Col<Feed, std::set<str>> feed_tags;
    db::Col<Feed, bool>          feed_active, feed_visible;
    db::Col<Feed, std::set<UId>> feed_peer_ids;
    db::Table<Feed> feeds, feeds_sort;

    db::Col<Post, UId>           post_id;
    db::Col<Post, UId>           post_feed_id;
    db::Col<Post, UId>           post_owner_id;
    db::Col<Post, Time>          post_created_at, post_changed_at;
    db::Col<Post, str>           post_body;
    db::Col<Post, std::set<str>> post_tags;
    db::Col<Post, std::set<UId>> post_peer_ids;
    db::Table<Post> posts, posts_sort, feed_posts;
    
    db::Col<Msg, UId>              msg_id;
    db::Col<Msg, str>              msg_type;
    db::Col<Msg, str>              msg_from, msg_to;
    db::Col<Msg, UId>              msg_from_id, msg_to_id;
    db::Col<Msg, Time>             msg_fetched_at;
    db::Col<Msg, str>              msg_peer_name;
    db::Col<Msg, crypt::PubKey>    msg_crypt_key;
    db::Col<Msg, db::Rec<Feed>>    msg_feed;
    db::Col<Msg, db::Rec<Post>>    msg_post;
    db::Col<Msg, db::Rec<Project>> msg_project;
    db::Col<Msg, db::Rec<Task>>    msg_task;
    db::Table<Msg> inbox, inbox_sort, outbox;

    db::Col<Project, UId>           project_id;
    db::Col<Project, UId>           project_owner_id;
    db::Col<Project, Time>          project_created_at, project_changed_at;
    db::Col<Project, str>           project_name, project_info;
    db::Col<Project, std::set<str>> project_tags;
    db::Col<Project, bool>          project_active;
    db::Col<Project, std::set<UId>> project_peer_ids;
    db::Table<Project> projects, projects_sort;

    db::Col<Task, UId>           task_id;
    db::Col<Task, UId>           task_project_id;
    db::Col<Task, UId>           task_owner_id;
    db::Col<Task, Time>          task_created_at, task_changed_at;
    db::Col<Task, str>           task_name, task_info;
    db::Col<Task, std::set<str>> task_tags;
    db::Col<Task, int64_t>       task_prio;
    db::Col<Task, bool>          task_done;
    db::Col<Task, std::set<UId>> task_peer_ids;
    db::Table<Task> tasks, tasks_sort;

    Db(Ctx &ctx);
  };
}

#endif
