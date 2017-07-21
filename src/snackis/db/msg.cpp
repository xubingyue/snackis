#include "snackis/db/msg.hpp"

namespace snackis {
namespace db {
  MsgFld<Chan<Msg> *> Msg::SENDER("sender");
  MsgFld<Changes> Msg::CHANGES("changes");

  BasicMsgFld::BasicMsgFld(const str id):
    id(id)
  { }

  Msg::Msg(MsgType t):
    type(t)
  { }
}}
