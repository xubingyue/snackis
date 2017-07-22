#include "snackis/db/basic_table.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/db/proc.hpp"
#include "snackis/db/change_loop.hpp"

namespace snackis {
namespace db {
  ChangeLoop::ChangeLoop(Proc &p, size_t max_buf):
    Loop(p, max_buf)
  { }

  void ChangeLoop::on_msg(const Msg &msg) {
    auto ctx(get(msg, Msg::SENDER));

    switch (msg.type) {
    case MSG_COMMIT: {
      auto cs(get(msg, Msg::CHANGES));
      
      for (auto q: queues) {
	if (q.first != ctx) {
	  std::copy(cs.begin(), cs.end(), std::back_inserter(q.second));
	}
      }

      break;
    }
    case MSG_REFRESH: {
      auto &q(queues[ctx]);
      Msg msg(MSG_OK);
      set(msg, Msg::CHANGES, q);
      put(ctx->inbox, msg);
      q.clear();
      break;
    }
    default:
      log(proc, "Unsupported message type: %0", msg.type);
    }
  }
}}
