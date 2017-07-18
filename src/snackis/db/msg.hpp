#ifndef SNACKIS_DB_MSG_HPP
#define SNACKIS_DB_MSG_HPP

#include <map>
#include <variant>
#include <vector>

#include "snackis/core/uid.hpp"
#include "snackis/db/change.hpp"

namespace snackis {
namespace db {
  struct Msg {
    enum Type { MSG_CONNECT, MSG_DISCONNECT };
    enum Field { MSG_CHAN, MSG_CHANGES };
    using Value = std::variant<UId, ChangeSet>;
    
    const Type type;
    std::map<Field, Value> values;
    
    Msg(Type t);
  };
}}

#endif
