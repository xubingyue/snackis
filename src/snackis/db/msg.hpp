#ifndef SNACKIS_DB_MSG_HPP
#define SNACKIS_DB_MSG_HPP

#include <map>
#include <variant>
#include <vector>

#include "snackis/core/uid.hpp"
#include "snackis/db/change.hpp"

namespace snackis {
namespace db {
  template <typename T>
  struct Chan {
  };

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

  struct Msg {
    enum Type { MSG_CONNECT, MSG_DISCONNECT, MSG_COMMIT };
    using Val = std::variant<Chan<Msg>, Changes>;

    static MsgFld<Chan<Msg>> CHAN;
    static MsgFld<Changes> CHANGES;

    const Type type;
    std::map<BasicMsgFld *, Val> vals;
    
    Msg(Type t);
  };
}}

#endif
