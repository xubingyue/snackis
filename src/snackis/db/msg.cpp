#include "snackis/db/msg.hpp"

namespace snackis {
namespace db {
  const MsgFld<Changes> Msg::CHANGES("changes");
  const MsgFld<int64_t> Msg::RECLAIMED("reclaimed");
  const MsgFld<Ctx *> Msg::SENDER("sender");

  BasicMsgFld::BasicMsgFld(const str id):
    id(id)
  { }

  Msg::Msg(MsgType t):
    type(t)
  { }
}}
