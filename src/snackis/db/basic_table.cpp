#include "snackis/core/fmt.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/db/basic_table.hpp"

namespace snackis {  
namespace db {
  BasicTable::BasicTable(Ctx &ctx, const str &name):
    ctx(ctx),
    name(name),
    path(get_path(ctx, name + ".tbl")) { }
}}
