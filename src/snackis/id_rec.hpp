#ifndef SNACKIS_ID_REC_HPP
#define SNACKIS_ID_REC_HPP

#include "snackis/rec.hpp"

namespace snackis {
  struct IdRec: Rec {
    UId id;
    
    IdRec(Ctx &ctx, opt<UId> id=nullopt);
  };

  str id_str(const IdRec &rec);
}

#endif
