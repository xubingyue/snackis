#ifndef SNACKIS_DB_WRITE_LOOP_HPP
#define SNACKIS_DB_WRITE_LOOP_HPP

#include <fstream>
#include <map>

#include "snackis/core/path.hpp"
#include "snackis/db/loop.hpp"

namespace snackis {
namespace db {
  struct Proc;
  
  struct WriteLoop: Loop {
    std::map<Path, std::ofstream> files;
    
    WriteLoop(Proc &p, size_t max_buf);
    ~WriteLoop();
    void on_msg(const Msg &msg) override;
  };
}}

#endif
