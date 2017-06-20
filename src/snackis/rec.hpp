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

  template <typename RecT>
  void copy(RecT &dest, const db::Rec<RecT> &src) {
    for (auto &f: src) { f.first->set(dest, f.second); }
  }
}

#endif
