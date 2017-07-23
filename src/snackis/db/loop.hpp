#ifndef SNACKIS_DB_LOOP_HPP
#define SNACKIS_DB_LOOP_HPP

#include <thread>
#include "snackis/core/chan.hpp"
#include "snackis/db/msg.hpp"

namespace snackis {
namespace db {
  struct Proc;
  
  struct Loop {
    Proc &proc;
    Chan<Msg> inbox;
    std::thread thread;
    
    Loop(Proc &proc, size_t max_buf);
    virtual void on_msg(const Msg &msg)=0;
  };

  void start(Loop &lp);
  void stop(Loop &lp);
}}

#endif
