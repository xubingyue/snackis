#include "snackis/db/ctx.hpp"
#include "snackis/db/proc.hpp"

namespace snackis {
namespace db {
  Proc::Proc(const Path &p, size_t max_buf):
    Loop(*this, max_buf),
    path(p),
    write_loop(*this, max_buf)
  {
    create_path(path);
  }

  void Proc::on_msg(const Msg &msg) {
    auto ctx(get(msg, Msg::SENDER));
    
    switch (msg.type) {
    case MSG_CONNECT:
      put(ctx->inbox, Msg(MSG_OK));      
      break;
    case MSG_DISCONNECT:
      put(ctx->inbox, Msg(MSG_OK));      
      break;
    case MSG_COMMIT:
    case MSG_REWRITE:
      put(write_loop.inbox, msg);
      break;
    default:
      log(*this, "Invalid message type: %0", msg.type);
    }
  }
}}
