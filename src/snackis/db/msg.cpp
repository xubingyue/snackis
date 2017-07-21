#include "snackis/db/msg.hpp"

namespace snackis {
namespace db {
  const MsgFld<Ctx *> Msg::SENDER("sender");
  const MsgFld<Changes> Msg::CHANGES("changes");

  BasicMsgFld::BasicMsgFld(const str id):
    id(id)
  { }

  Msg::Msg(MsgType t):
    type(t)
  { }
}}
