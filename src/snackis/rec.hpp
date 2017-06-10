#ifndef SNACKIS_REC_HPP
#define SNACKIS_REC_HPP

#include "snackis/core/uid_type.hpp"
#include "snackis/core/vector_type.hpp"
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

  using RecVector = std::vector<UId>;
  extern VectorType<UId> rec_vector_type;
}

#endif
