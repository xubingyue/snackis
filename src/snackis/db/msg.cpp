#include "snackis/db/msg.hpp"

namespace snackis {
namespace db {
  MsgFld<Chan<Msg>> Msg::CHAN("chan");
  MsgFld<Changes> Msg::CHANGES("changes");

  BasicMsgFld::BasicMsgFld(const str id):
    id(id)
  { }

  Msg::Msg(Type t):
    type(t)
  { }
}}
