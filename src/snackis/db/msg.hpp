#ifndef SNACKIS_DB_MSG_HPP
#define SNACKIS_DB_MSG_HPP

#include <map>
#include <variant>
#include <vector>

#include "snackis/core/chan.hpp"
#include "snackis/db/change.hpp"

namespace snackis {
namespace db {
  struct BasicMsgFld {
    const str id;
    BasicMsgFld(const str id);
  };

  template <typename ValT>
  struct MsgFld: BasicMsgFld {
    MsgFld(const str id);
  };

  template <typename ValT>
  MsgFld<ValT>::MsgFld(const str id):
    BasicMsgFld(id)
  { }

  enum MsgType { MSG_CONNECT, MSG_DISCONNECT, MSG_COMMIT };

  struct Msg {
    using Val = std::variant<Chan<Msg> *, Changes>;

    static MsgFld<Chan<Msg> *> SENDER;
    static MsgFld<Changes> CHANGES;

    const MsgType type;
    std::map<BasicMsgFld *, Val> vals;
    
    Msg(MsgType t);
  };
}}

#endif
