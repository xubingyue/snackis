#ifndef SNACKIS_DB_MSG_HPP
#define SNACKIS_DB_MSG_HPP

#include <map>
#include <variant>
#include <vector>

#include "snackis/core/error.hpp"
#include "snackis/db/change.hpp"

namespace snackis {
namespace db {
  struct Ctx;
  
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

  enum MsgType { MSG_CONNECT, MSG_DISCONNECT,
		 MSG_OK, MSG_ERROR };

  struct Msg {
    using Val = std::variant<Ctx *, Changes>;

    static const MsgFld<Ctx *> SENDER;
    static const MsgFld<Changes> CHANGES;
    
    const MsgType type;
    std::map<const BasicMsgFld *, Val> vals;
    
    Msg(MsgType t);
  };

  template <typename ValT>
  opt<ValT> find(const Msg &msg, const MsgFld<ValT> &fld) {
    auto fnd(msg.vals.find(&fld));
    if (fnd == msg.vals.end()) { return nullopt; }
    return std::get<ValT>(fnd->second);
  }

  template <typename ValT>
  ValT get(const Msg &msg, const MsgFld<ValT> &fld) {
    auto fnd(find(msg, fld));
    CHECK(fnd, _);
    return *fnd;
  }

  template <typename ValT>
  void set(Msg &msg, const MsgFld<ValT> &fld, ValT val) {
    msg.vals[&fld] = val;
  }
}}

#endif
