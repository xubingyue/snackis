#include "snackis/ctx.hpp"
#include "snackis/post.hpp"

namespace snackis {
  Post::Post(Ctx &ctx, const db::Rec<Thread> &thread):
    Rec(ctx), thread(thread)
  { }

  Post::Post(const db::Table<Post> &tbl, const db::Rec<Post> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), id(false) {
    copy(tbl, *this, rec);
  }
}
