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
  static void init_indexes(Db &db) {
    db.peers.indexes.insert(&db.peers_sort);
    db.scripts.indexes.insert(&db.scripts_sort);
    db.feeds.indexes.insert(&db.feeds_sort);
    db.posts.indexes.insert(&db.posts_sort);
    db.posts.indexes.insert(&db.feed_posts);
    db.inbox.indexes.insert(&db.inbox_sort);
    db.projects.indexes.insert(&db.projects_sort);
    db.tasks.indexes.insert(&db.tasks_sort);
  }

  static void init_events(Db &db, Ctx &ctx) {
    db.peers.on_update.push_back([&](auto &prev_rec, auto &curr_rec) {
	db::set(curr_rec, peer_changed_at, now());
      });

    db.scripts.on_insert.push_back([&](auto &rec) {
	Script sct(ctx, rec);
	if (sct.owner_id == whoamid(ctx)) { send(sct); }
	db::copy(db.scripts, rec, sct);
      });

    db.scripts.on_update.push_back([&](auto &prev_rec, auto &curr_rec) {
	Script curr(ctx, curr_rec), prev(ctx, prev_rec);
	curr.changed_at = now();
	auto fd(find_feed_id(ctx, curr.id));
	
	if (fd && fd->peer_ids != curr.peer_ids) {
	  fd->peer_ids = curr.peer_ids;
	  db::update(db.feeds, *fd);
	}

	if (curr.owner_id == whoamid(ctx)) {
	  if (db::compare(db.scripts_share, curr, prev) != 0) {
	    send(curr);
	  } else {
	    std::set<UId> added;
	    std::set_difference(curr.peer_ids.begin(), curr.peer_ids.end(),
				prev.peer_ids.begin(), prev.peer_ids.end(),
				std::inserter(added, added.end()));
	    for (auto &id: added) { send(curr, get_peer_id(ctx, id)); }
	  }
	}

	db::copy(db.scripts, curr_rec, curr);
      });

    db.feeds.on_update.push_back([&](auto &prev_rec, auto &curr_rec) {	
	db::set(curr_rec, feed_changed_at, now());
      });

    db.posts.on_insert.push_back([&](auto &rec) {
	Post ps(ctx, rec);
	if (ps.owner_id == whoamid(ctx)) { send(ps); }
	db::copy(db.posts, rec, ps);
      });

    db.posts.on_update.push_back([&](auto &prev_rec, auto &curr_rec) {
	Post curr(ctx, curr_rec), prev(ctx, prev_rec);
	curr.changed_at = now();

	auto fd(find_feed_id(ctx, curr.id));
	if (fd && fd->peer_ids != curr.peer_ids) {		    	
	  fd->peer_ids = curr.peer_ids;
	  db::update(db.feeds, *fd);
	}

	if (curr.owner_id == whoamid(ctx)) {
	  if (db::compare(db.posts_share, curr, prev) != 0) {
	    send(curr);
	  } else {
	    std::set<UId> added;
	    std::set_difference(curr.peer_ids.begin(), curr.peer_ids.end(),
				prev.peer_ids.begin(), prev.peer_ids.end(),
				std::inserter(added, added.end()));
	    for (auto &id: added) { send(curr, get_peer_id(ctx, id)); }
	  }
	}
	
	db::copy(db.posts, curr_rec, curr);
      });

    db.projects.on_update.push_back([&](auto &prev_rec, auto &curr_rec) {
	Project curr(ctx, curr_rec);
	curr.changed_at = now();
	auto fd(find_feed_id(ctx, curr.id));
	
	if (fd && fd->peer_ids != curr.peer_ids) {
	  fd->peer_ids = curr.peer_ids;
	  db::update(db.feeds, *fd);
	}

	db::copy(db.projects, curr_rec, curr);
      });

    db.tasks.on_insert.push_back([&](auto &rec) {
	Task tsk(ctx, rec);
	if (tsk.owner_id == whoamid(ctx)) { send(tsk); }
	if (tsk.done) { tsk.done_at = now(); }
	db::copy(db.tasks, rec, tsk);
      });

    db.tasks.on_update.push_back([&](auto &prev_rec, auto &curr_rec) {
	Task curr(ctx, curr_rec), prev(ctx, prev_rec);
	curr.changed_at = now();
	if (curr.done && !prev.done) { curr.done_at = now(); }
	
	auto fd(find_feed_id(ctx, curr.id));
	if (fd && fd->peer_ids != curr.peer_ids) {
	  fd->peer_ids = curr.peer_ids;
	  db::update(db.feeds, *fd);
	}
	
	if (curr.owner_id == whoamid(ctx)) {
	  if (db::compare(db.tasks_share, curr, prev) != 0) {
	    send(curr);
	  } else {
	    std::set<UId> added;
	    std::set_difference(curr.peer_ids.begin(), curr.peer_ids.end(),
				prev.peer_ids.begin(), prev.peer_ids.end(),
				std::inserter(added, added.end()));
	    for (auto &id: added) { send(curr, get_peer_id(ctx, id)); }
	  }
	}

	db::copy(db.tasks, curr_rec, curr);
      });
  }
  
  Db::Db(Ctx &ctx):
    settings(ctx, "settings", db::make_key(setting_key), {&setting_val}),

    invites(ctx, "invites", db::make_key(invite_to), {&invite_posted_at}),
    
    peers(ctx, "peers", db::make_key(peer_id),
	  {&peer_created_at, &peer_changed_at, &peer_name, &peer_email, &peer_info,
	      &peer_tags, &peer_crypt_key, &peer_active}),

    peers_sort(ctx, "peers_sort", db::make_key(peer_name, peer_id), {}),
    
    scripts(ctx, "scripts", script_key, script_cols),

    scripts_sort(ctx, "scripts_sort",
		 db::make_key(script_name, script_created_at, script_id),
		 {}),

    scripts_share({&script_id, &script_created_at, &script_changed_at, &script_name,
	  &script_code, &script_peer_ids}),

    feeds(ctx, "feeds", feed_key, feed_cols),

    feeds_sort(ctx, "feeds_sort", db::make_key(feed_created_at, feed_id), {}),

    feeds_share({&feed_id, &feed_created_at, &feed_changed_at, &feed_name,
	  &feed_info, &feed_active, &feed_visible, &feed_peer_ids}),
    
    posts(ctx, "posts", post_key, post_cols),

    posts_sort(ctx, "posts_sort", db::make_key(post_created_at, post_id), {}),

    feed_posts(ctx,
	       "feed_posts",
	       db::make_key(post_feed_id, post_created_at, post_id),
	       {}),

    posts_share({&post_id, &post_feed_id, &post_created_at, &post_changed_at,
	  &post_body, &post_peer_ids}),
    
    inbox(ctx, "inbox", db::make_key(msg_id),
	  {&msg_type, &msg_fetched_at, &msg_peer_name, &msg_from, &msg_from_id,
	      &msg_crypt_key, &msg_script, &msg_feed, &msg_post, &msg_project,
	      &msg_task}),
    
    outbox(ctx, "outbox", db::make_key(msg_id),
	   {&msg_type, &msg_from, &msg_from_id, &msg_to, &msg_to_id, &msg_peer_name,
	       &msg_crypt_key, &msg_script, &msg_feed, &msg_post, &msg_project,
	       &msg_task}),

    inbox_sort(ctx, "inbox_sort", db::make_key(msg_fetched_at, msg_id), {}),

    projects(ctx, "projects", project_key, project_cols),

    projects_sort(ctx, "projects_sort", db::make_key(project_name, project_id), {}),

    projects_share({&project_id, &project_created_at, &project_changed_at,
	  &project_name, &project_info, &project_active, &project_peer_ids}),
    
    tasks(ctx, "tasks", task_key, task_cols),

    tasks_sort(ctx, "tasks_sort", db::make_key(task_prio, task_created_at, task_id),
	       {}),

    tasks_share({&task_id, &task_created_at, &task_changed_at, &task_project_id,
	  &task_name, &task_info, &task_done, &task_done_at, &task_peer_ids})
      
  {
    init_indexes(*this);
    init_events(*this, ctx);
  }
}
