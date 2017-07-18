#ifndef SNACKIS_DB_LOOP_HPP
#define SNACKIS_DB_LOOP_HPP

#include "snackis/db/msg.hpp"

namespace snackis {
namespace db {
  struct Proc;
  
  struct Loop {
    Proc &proc;
    Chan<Msg> inbox;
    Loop(Proc &proc, size_t max_buf);
  };
}}

#endif
