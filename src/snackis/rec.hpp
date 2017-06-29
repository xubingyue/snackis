#ifndef SNACKIS_REC_HPP
#define SNACKIS_REC_HPP

#include "snackis/db/rec.hpp"

namespace snackis {
  namespace db {
    struct Ctx;
    
    template <typename RecT>
    struct Table;
  }

  struct Ctx;
  
  struct Rec {
    Ctx &ctx;

    Rec(Ctx &ctx);
  };
}

#endif
