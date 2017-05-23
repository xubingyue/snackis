#ifndef SNACKIS_REC_HPP
#define SNACKIS_REC_HPP

namespace snackis {
  struct Ctx;
  
  struct Rec {
    Ctx &ctx;

    Rec(Ctx &ctx);
  };
}

#endif
