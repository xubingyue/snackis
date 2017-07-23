#ifndef SNACKIS_DB_CHANGE_LOOP_HPP
#define SNACKIS_DB_CHANGE_LOOP_HPP

#include <fstream>
#include <map>

#include "snackis/core/path.hpp"
#include "snackis/db/change.hpp"
#include "snackis/db/loop.hpp"

namespace snackis {
namespace db {
  struct Proc;
  
  struct ChangeLoop: Loop {
    std::map<Ctx *, Changes> queues;
    
    ChangeLoop(Proc &p, size_t max_buf);
    ~ChangeLoop();
    void on_msg(const Msg &msg) override;
  };
}}

#endif
