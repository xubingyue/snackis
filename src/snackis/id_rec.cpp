#include "snackis/id_rec.hpp"

namespace snackis {
  IdRec::IdRec(Ctx &ctx, opt<UId> id): Rec(ctx), id(id ? *id : true)
  { }

  str id_str(const IdRec &rec) {
    return to_str(rec.id).substr(0, 8);
  }
}
