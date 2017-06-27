#include "snackis/ctx.hpp"
#include "snackis/db.hpp"
#include "snackis/core/diff.hpp"
#include "snackis/core/prim_type.hpp"
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
    peer_name(      "name",       str_type,            &Peer::name),
    peer_email(     "email",      str_type,            &Peer::email),
    peer_crypt_key( "crypt_key",  crypt::pub_key_type, &Peer::crypt_key),

    peers(ctx, "peers", {&peer_id},
	  {&peer_name, &peer_email, &peer_crypt_key}),

    peers_sort(ctx, "peers_sort", {&peer_name, &peer_id}, {}),
    
    feed_id(        "id",         uid_type,     &Feed::id),
    feed_owner_id(  "owner_id",   uid_type,     &Feed::owner_id),
    feed_created_at("created_at", time_type,    &Feed::created_at),
    feed_name(      "name",       str_type,     &Feed::name),
    feed_info(      "into",       str_type,     &Feed::info),
    feed_active(    "active",     bool_type,    &Feed::active),
    feed_visible(   "visible",    bool_type,    &Feed::visible),
    feed_peer_ids(  "peer_ids",   uid_set_type, &Feed::peer_ids),
    
    feeds(ctx, "feeds", {&feed_id},
	  {&feed_owner_id, &feed_created_at, &feed_name, &feed_info, &feed_active,
	      &feed_visible, &feed_peer_ids}),

    feeds_sort(ctx, "feeds_sort", {&feed_created_at, &feed_id}, {}),
		  
    post_id(     "id",      uid_type,  &Post::id),
    post_feed_id("feed_id", uid_type,  &Post::feed_id),
    post_at(     "at",      time_type, &Post::at),
    post_by_id(  "by_id",   uid_type,  &Post::by_id),
    post_body(   "body",    str_type,  &Post::body),

    posts(ctx, "posts", {&post_id},
	  {&post_feed_id, &post_at, &post_by_id, &post_body}),

    posts_sort(ctx, "posts_sort", {&post_at, &post_id}, {}),
    feed_posts(ctx, "feed_posts", {&post_feed_id, &post_at, &post_id}, {}),
    
    msg_id(          "id",           uid_type,            &Msg::id),
    msg_type(        "type",         str_type,            &Msg::type),
    msg_from(        "from",         str_type,            &Msg::from),
    msg_to(          "to",           str_type,            &Msg::to),
    msg_from_id(     "from_id",      uid_type,            &Msg::from_id),
    msg_to_id(       "to_id",        uid_type,            &Msg::to_id),
    msg_fetched_at(  "fetched_at",   time_type,           &Msg::fetched_at),
    msg_peer_name(   "peer_name",    str_type,            &Msg::peer_name),
    msg_crypt_key(   "crypt_key",    crypt::pub_key_type, &Msg::crypt_key),
    msg_feed_id(     "feed_id",      uid_type,            &Msg::feed_id),
    msg_feed_name(   "feed_name",    str_type,            &Msg::feed_name),
    msg_feed_info(   "feed_info",    str_type,            &Msg::feed_info),
    msg_feed_visible("feed_visible", bool_type,           &Msg::feed_visible),
    msg_post_id(     "post_id",      uid_type,            &Msg::post_id),
    msg_post_at(     "post_at",      time_type,           &Msg::post_at),
    msg_post_body(   "post_body",    str_type,            &Msg::post_body),
    
    inbox(ctx, "inbox", {&msg_id},
	  {&msg_type, &msg_fetched_at, &msg_peer_name, &msg_from, &msg_from_id,
	      &msg_crypt_key, &msg_feed_id, &msg_feed_name, &msg_feed_info,
	      &msg_feed_visible, &msg_post_id, &msg_post_at, &msg_post_body}),

    inbox_sort(ctx, "inbox_sort", {&msg_fetched_at, &msg_id}, {}),
    
    outbox(ctx, "outbox", {&msg_id},
	   {&msg_type, &msg_from, &msg_from_id, &msg_to, &msg_to_id, &msg_peer_name,
	       &msg_crypt_key, &msg_feed_id, &msg_feed_name, &msg_feed_info,
	       &msg_feed_visible, &msg_post_id, &msg_post_at, &msg_post_body}),

    project_id(      "id",       uid_type,     &Project::id),
    project_owner_id("owner_id", uid_type,     &Project::owner_id),
    project_name(    "name",     str_type,     &Project::name),
    project_info(    "info",     str_type,     &Project::info),
    project_active(  "active",   bool_type,    &Project::active),
    project_peer_ids("peer_ids", uid_set_type, &Project::peer_ids),
    
    projects(ctx, "projects", {&project_id},
	     {&project_owner_id, &project_name, &project_info, &project_active,
		 &project_peer_ids}),

    projects_sort(ctx, "projects_sort", {&project_name, &project_id}, {}),
    
    task_id(        "id",         uid_type,     &Task::id),
    task_project_id("project_id", uid_type,     &Task::project_id),
    task_owner_id(  "owner_id",   uid_type,     &Task::owner_id),
    task_created_at("created_at", time_type,    &Task::created_at),
    task_name(      "name",       str_type,     &Task::name),
    task_info(      "info",       str_type,     &Task::info),
    task_done(      "done",       bool_type,    &Task::done),
    task_peer_ids(  "peer_ids",   uid_set_type, &Task::peer_ids),
    task_queue_ids( "queue_ids",  uid_set_type, &Task::queue_ids),
    
    tasks(ctx, "tasks", {&task_id},
	  {&task_project_id, &task_owner_id, &task_created_at, &task_name,
	      &task_info, &task_done, &task_peer_ids, &task_queue_ids}),

    tasks_sort(ctx, "tasks_sort", {&task_created_at, &task_id}, {}),
    
    queue_id(      "id",       uid_type,     &Queue::id),
    queue_owner_id("owner_id", uid_type,     &Queue::owner_id),
    queue_name(    "name",     str_type,     &Queue::name),
    queue_info(    "info",     str_type,     &Queue::info),
    queue_task_ids("task_ids", uid_set_type, &Queue::task_ids),
    
    queues(ctx, "queues", {&queue_id},
	   {&queue_owner_id, &queue_name, &queue_info, &queue_task_ids}),

    queues_sort(ctx, "queues_sort", {&queue_name, &queue_id}, {}),

    queue_task_id(      "id",       uid_type,  &QueueTask::id),
    queue_task_queue_id("queue_id", uid_type,  &QueueTask::queue_id),
    queue_task_at(      "at",       time_type, &QueueTask::at),
    
    queue_tasks(ctx, "queue_tasks",
		{&queue_task_queue_id, &queue_task_at, &queue_task_id},
		{})
  {
    peers.indexes.insert(&peers_sort);
    inbox.indexes.insert(&inbox_sort);
    feeds.indexes.insert(&feeds_sort);
    posts.indexes.insert(&posts_sort);
    posts.indexes.insert(&feed_posts);
    projects.indexes.insert(&projects_sort);
    tasks.indexes.insert(&tasks_sort);
    queues.indexes.insert(&queues_sort);

    projects.on_update.push_back([&](auto prev_rec, auto curr_rec) {
	Project prev(ctx, prev_rec), curr(ctx, curr_rec);

	if (curr.peer_ids != prev.peer_ids) {
	  auto feed(find_feed_id(ctx, curr.id));

	  if (feed) {
	    feed->peer_ids = curr.peer_ids;
	    db::update(feeds, *feed);
	  }
	}
      });

    tasks.on_update.push_back([&](auto prev_rec, auto curr_rec) {
	Task prev(ctx, prev_rec), curr(ctx, curr_rec);

	if (curr.peer_ids != prev.peer_ids) {		    	
	  auto feed(find_feed_id(ctx, curr.id));

	  if (feed) {
	    feed->peer_ids = curr.peer_ids;
	    db::update(feeds, *feed);
	  }
	}
      });
  }
}
