#include "snackis/db/basic_table.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/db/proc.hpp"
#include "snackis/db/write_loop.hpp"

namespace snackis {
namespace db {
  WriteLoop::WriteLoop(Proc &p, size_t max_buf):
    Loop(p, max_buf)
  {
    start(*this);
  }

  static std::ofstream &get_file(WriteLoop &lp, const Path &p) {
    auto fnd(lp.files.find(p));

    if (fnd == lp.files.end()) {
      auto &f(lp.files.emplace(p, 
			       std::ofstream(p.string(),
					     std::ios::out | 
					     std::ios::binary | 
					     std::ios::app)).first->second);
      if (f.fail()) {
	lp.files.erase(p);
	ERROR(Db, fmt("Failed opening file: %0", p.string()));
      }

      return f;
    }

    return fnd->second;
  }

  void WriteLoop::on_msg(const Msg &msg) {
    auto ctx(get(msg, Msg::SENDER));

    switch (msg.type) {
    case MSG_COMMIT: { 
      std::set<std::ofstream *> dirty;

      for (auto &c: get(msg, Msg::CHANGES)) {
	auto &f(get_file(*this, c->table_path()));

	if (!f.fail()) {
	  c->write(f);
	  dirty.insert(&f);
	}
      }

      for (auto f: dirty) { f->flush(); }
      break;
    }
    case MSG_REWRITE: {
      int64_t reclaimed(0); 
      
      for (auto t: ctx->tables) {
	auto &f(get_file(*this, t.second->path));
	auto old_size(f.tellp());
	f.close();
	f.open(t.second->path.string(), 
	       std::ios::out | std::ios::binary | std::ios::trunc);
	t.second->dump(f);
	reclaimed += old_size-f.tellp();
      }

      Msg msg(MSG_OK);
      set(msg, Msg::RECLAIMED, reclaimed);
      put(ctx->inbox, msg);
      break;
    }
    default:
      log(proc, "Unsupported message type: %0", msg.type);
    }
  }
}}
