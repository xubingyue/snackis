#include "snackis/ctx.hpp"
#include "snackis/db.hpp"
#include "snackis/core/diff.hpp"
#include "snackis/core/bool_type.hpp"
#include "snackis/core/int64_type.hpp"
#include "snackis/core/set_type.hpp"
#include "snackis/core/str_type.hpp"
#include "snackis/core/time_type.hpp"
#include "snackis/core/uid_type.hpp"
#include "snackis/crypt/pub_key_type.hpp"

namespace snackis {  
  Db::Db(Ctx &ctx):
    setting_key("key", str_type, &BasicSetting::key),
    setting_val("val", str_type, &BasicSetting::val),
    
    settings(ctx, "settings",
	     {&setting_key}, {&setting_val}),

    invite_to(       "to",        str_type,  &Invite::to),
    invite_posted_at("posted_at", time_type, &Invite::posted_at),

    invites(ctx, "invites",
	    {&invite_to}, {&invite_posted_at}),
    
    peer_id(        "id",         uid_type,            &Peer::id),
    peer_created_at("created_at", time_type,           &Peer::created_at),
    peer_changed_at("changed_at", time_type,           &Peer::changed_at),
    peer_name(      "name",       str_type,            &Peer::name),
    peer_email(     "email",      str_type,            &Peer::email),
    peer_crypt_key( "crypt_key",  crypt::pub_key_type, &Peer::crypt_key),

    peers(ctx, "peers", {&peer_id},
	  {&peer_created_at, &peer_changed_at, &peer_name, &peer_email,
	      &peer_crypt_key}),

    peers_sort(ctx, "peers_sort", {&peer_name, &peer_id}, {}),
    
    feed_id(        "id",         uid_type,     &Feed::id),
    feed_owner_id(  "owner_id",   uid_type,     &Feed::owner_id),
    feed_created_at("created_at", time_type,    &Feed::created_at),
    feed_changed_at("changed_at", time_type,    &Feed::changed_at),
    feed_name(      "name",       str_type,     &Feed::name),
    feed_info(      "into",       str_type,     &Feed::info),
    feed_tags(      "tags",       str_set_type, &Feed::tags),
    feed_active(    "active",     bool_type,    &Feed::active),
    feed_visible(   "visible",    bool_type,    &Feed::visible),
    feed_peer_ids(  "peer_ids",   uid_set_type, &Feed::peer_ids),
    
    feeds(ctx, "feeds", {&feed_id},
	  {&feed_owner_id, &feed_created_at, &feed_changed_at, &feed_name,
	      &feed_info, &feed_tags, &feed_active, &feed_visible, &feed_peer_ids}),

    feeds_sort(ctx, "feeds_sort", {&feed_created_at, &feed_id}, {}),
		  
    post_id(        "id",         uid_type,     &Post::id),
    post_feed_id(   "feed_id",    uid_type,     &Post::feed_id),
    post_owner_id(  "owner_id",   uid_type,     &Post::owner_id),
    post_created_at("created_at", time_type,    &Post::created_at),
    post_changed_at("changed_at", time_type,    &Post::changed_at),
    post_body(      "body",       str_type,     &Post::body),
    post_tags(      "tags",       str_set_type, &Post::tags),
    post_peer_ids(  "peer_ids",   uid_set_type, &Post::peer_ids),

    posts(ctx, "posts", {&post_id},
	  {&post_feed_id, &post_owner_id, &post_created_at, &post_changed_at,
	      &post_body, &post_tags, &post_peer_ids}),

    posts_sort(ctx, "posts_sort", {&post_created_at, &post_id}, {}),
    feed_posts(ctx, "feed_posts", {&post_feed_id, &post_created_at, &post_id}, {}),
    
    msg_id(        "id",         uid_type,            &Msg::id),
    msg_type(      "type",       str_type,            &Msg::type),
    msg_from(      "from",       str_type,            &Msg::from),
    msg_to(        "to",         str_type,            &Msg::to),
    msg_from_id(   "from_id",    uid_type,            &Msg::from_id),
    msg_to_id(     "to_id",      uid_type,            &Msg::to_id),
    msg_fetched_at("fetched_at", time_type,           &Msg::fetched_at),
    msg_peer_name( "peer_name",  str_type,            &Msg::peer_name),
    msg_crypt_key( "crypt_key",  crypt::pub_key_type, &Msg::crypt_key),
    msg_feed(      "feed",       feeds.rec_type,      &Msg::feed),
    msg_post(      "post",       posts.rec_type,      &Msg::post),
    msg_project(   "project",    projects.rec_type,   &Msg::project),
    msg_task(      "task",       tasks.rec_type,      &Msg::task),
    
    inbox(ctx, "inbox", {&msg_id},
	  {&msg_type, &msg_fetched_at, &msg_peer_name, &msg_from, &msg_from_id,
	      &msg_crypt_key, &msg_feed, &msg_post, &msg_project, &msg_task}),

    inbox_sort(ctx, "inbox_sort", {&msg_fetched_at, &msg_id}, {}),
    
    outbox(ctx, "outbox", {&msg_id},
	   {&msg_type, &msg_from, &msg_from_id, &msg_to, &msg_to_id, &msg_peer_name,
	       &msg_crypt_key, &msg_feed, &msg_post, &msg_project, &msg_task}),

    project_id(        "id",         uid_type,     &Project::id),
    project_owner_id(  "owner_id",   uid_type,     &Project::owner_id),
    project_created_at("created_at", time_type,    &Project::created_at),
    project_changed_at("changed_at", time_type,    &Project::changed_at),
    project_name(      "name",       str_type,     &Project::name),
    project_info(      "info",       str_type,     &Project::info),
    project_tags(      "tags",       str_set_type, &Project::tags),
    project_active(    "active",     bool_type,    &Project::active),
    project_peer_ids(  "peer_ids",   uid_set_type, &Project::peer_ids),
    
    projects(ctx, "projects", {&project_id},
	     {&project_owner_id, &project_created_at, &project_changed_at,
		 &project_name, &project_info, &project_tags, &project_active,
		 &project_peer_ids}),

    projects_sort(ctx, "projects_sort", {&project_name, &project_id}, {}),
    
    task_id(        "id",         uid_type,     &Task::id),
    task_project_id("project_id", uid_type,     &Task::project_id),
    task_owner_id(  "owner_id",   uid_type,     &Task::owner_id),
    task_created_at("created_at", time_type,    &Task::created_at),
    task_changed_at("changed_at", time_type,    &Task::changed_at),
    task_name(      "name",       str_type,     &Task::name),
    task_info(      "info",       str_type,     &Task::info),
    task_tags(      "tags",       str_set_type, &Task::tags),
    task_done(      "done",       bool_type,    &Task::done),
    task_peer_ids(  "peer_ids",   uid_set_type, &Task::peer_ids),
    
    tasks(ctx, "tasks", {&task_id},
	  {&task_project_id, &task_owner_id, &task_created_at, &task_changed_at,
	      &task_name, &task_info, &task_tags, &task_done, &task_peer_ids}),

    tasks_sort(ctx, "tasks_sort", {&task_created_at, &task_id}, {})
  {
    peers.indexes.insert(&peers_sort);
    inbox.indexes.insert(&inbox_sort);
    feeds.indexes.insert(&feeds_sort);
    posts.indexes.insert(&posts_sort);
    posts.indexes.insert(&feed_posts);
    projects.indexes.insert(&projects_sort);
    tasks.indexes.insert(&tasks_sort);

    peers.on_update.push_back([&](auto &prev_rec, auto &curr_rec) {
	db::set(curr_rec, peer_changed_at, now());
      });

    feeds.on_update.push_back([&](auto &prev_rec, auto &curr_rec) {	
	db::set(curr_rec, feed_changed_at, now());
      });

    posts.on_insert.push_back([&](auto &rec) {
	Post ps(ctx, rec);
	if (ps.owner_id == whoami(ctx).id) { send(ps); }
      });

    posts.on_update.push_back([&](auto &prev_rec, auto &curr_rec) {
	Post curr(ctx, curr_rec);
	curr.peer_ids = get_feed_id(ctx, curr.feed_id).peer_ids;
	curr.changed_at = now();
	db::copy(posts, curr_rec, curr);

	auto fd(find_feed_id(ctx, curr.id));
	if (fd && (fd->tags != curr.tags ||
		   fd->peer_ids != curr.peer_ids)) {		    	
	  fd->tags = curr.tags;
	  fd->peer_ids = curr.peer_ids;
	  db::update(feeds, *fd);
	}
	
	send(curr);
      });

    projects.on_update.push_back([&](auto &prev_rec, auto &curr_rec) {
	Project curr(ctx, curr_rec);
	auto fd(find_feed_id(ctx, curr.id));
	
	if (fd && (fd->tags != curr.tags ||
		   fd->peer_ids != curr.peer_ids)) {
	  fd->peer_ids = curr.peer_ids;
	  db::update(feeds, *fd);
	}

	db::set(curr_rec, project_changed_at, now());
      });

    tasks.on_insert.push_back([&](auto &rec) {
	Task tsk(ctx, rec);
	if (tsk.owner_id == whoami(ctx).id) { send(tsk); }
      });

    tasks.on_update.push_back([&](auto &prev_rec, auto &curr_rec) {
	Task curr(ctx, curr_rec);
	curr.peer_ids = get_project_id(ctx, curr.project_id).peer_ids;
	curr.changed_at = now();
	db::copy(tasks, curr_rec, curr);
	
	auto fd(find_feed_id(ctx, curr.id));
	if (fd && (fd->tags != curr.tags ||
		   fd->peer_ids != curr.peer_ids)) {
	  fd->peer_ids = curr.peer_ids;
	  db::update(feeds, *fd);
	}
	
	send(curr);
      });
  }
}
