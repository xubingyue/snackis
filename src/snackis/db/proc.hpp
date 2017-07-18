#ifndef SNACKIS_DB_PROC_HPP
#define SNACKIS_DB_PROC_HPP

#include "snackis/core/path.hpp"
#include "snackis/db/loop.hpp"

namespace snackis {
namespace db {
  struct Proc: Loop {
    const Path path;
    
    Proc(const Path &p, size_t max_buf);
  };
}}

#endif
