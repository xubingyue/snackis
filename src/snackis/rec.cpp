#include "snackis/rec.hpp"

namespace snackis {
  VectorType<UId> rec_vector_type(uid_type);

  Rec::Rec(Ctx &ctx): ctx(ctx) { }
}
