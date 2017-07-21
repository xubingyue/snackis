#include "snackis/db/proc.hpp"

namespace snackis {
namespace db {
  Proc::Proc(const Path &p, size_t max_buf):
    Loop(*this, max_buf), path(p)
  {
    create_path(path);
  }

  void Proc::on_msg(const Msg &msg) {
    switch (msg.type) {
    case MSG_CONNECT:
      log(*this, "Connect");
      break;
    case MSG_DISCONNECT:
      log(*this, "Disconnect");
      break;
    default:
      log(*this, "Invalid message type: %0", msg.type);
    }
  }
}}
