#ifndef SNACKIS_DB_BASIC_TABLE_HPP
#define SNACKIS_DB_BASIC_TABLE_HPP

#include "snackis/core/path.hpp"
#include "snackis/core/str.hpp"

namespace snackis {  
namespace db {
  struct Ctx;
  
  struct BasicTable {
    Ctx &ctx;
    const str name;
    const Path path;
    
    BasicTable(Ctx &ctx, const str &name);
    virtual void dump(std::ostream &out) = 0;
    virtual void slurp() = 0;
  };
}}

#endif
