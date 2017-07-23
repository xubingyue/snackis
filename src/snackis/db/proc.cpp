#include "snackis/snackis.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/proc.hpp"

namespace snackis {
namespace db {
  static bool init_db_rev(Proc &proc) {
    const Path p(proc.path / "rev");
    
    if (path_exists(p)) {
      std::ifstream in;
      in.open(p.string(), std::ios::in | std::ios::binary);
      int64_t rev = -1;
      in.read(reinterpret_cast<char *>(&rev), sizeof rev);
      in.close();

      if (rev < DB_REV) {
	ERROR(Db, fmt("This version of Snackis requires database revision #%0 to run",
		      DB_REV));
	return false;
      }

      return true;
    }
    
    std::ofstream out;
    out.open(p.string(), std::ios::out | std::ios::trunc | std::ios::binary);
    out.write(reinterpret_cast<const char *>(&DB_REV), sizeof(DB_REV));
    out.close();

    log(proc, "Initialized database, revision %0", DB_REV);
    return true;
  }

  Proc::Proc(const Path &p, size_t max_buf):
    Loop(*this, max_buf),
    path(p),
    write_loop(*this, max_buf),
    change_loop(*this, max_buf)
  {
    create_path(path);
    init_db_rev(*this);
    start(*this);
  }

  void Proc::on_msg(const Msg &msg) {
    auto ctx(get(msg, Msg::SENDER));
    
    switch (msg.type) {
    case MSG_CONNECT:
      put(ctx->inbox, Msg(MSG_OK));      
      put(change_loop.inbox, msg);
      break;
    case MSG_DISCONNECT:
      put(ctx->inbox, Msg(MSG_OK));      
      put(change_loop.inbox, msg);
      break;
    case MSG_COMMIT:
      put(write_loop.inbox, msg);
      put(change_loop.inbox, msg);
      break;
    case MSG_REFRESH:
      put(change_loop.inbox, msg);
      break;      
    case MSG_REWRITE:
      put(write_loop.inbox, msg);
      break;
    default:
      log(*this, "Invalid message type: %0", msg.type);
    }
  }
}}
