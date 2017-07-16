#ifndef SNACKIS_DB_BASIC_TABLE_HPP
#define SNACKIS_DB_BASIC_TABLE_HPP

#include <fstream>
#include "snackis/core/str.hpp"

namespace snackis {  
namespace db {
  struct Ctx;
  
  struct BasicTable {
    Ctx &ctx;
    const str name;
    std::fstream file;
    
    BasicTable(Ctx &ctx, const str &name);
    virtual void slurp() = 0;
    virtual int64_t rewrite() = 0;
  };

  void open(BasicTable &tbl, std::ios_base::openmode mod=std::ios::app);
}}

#endif
