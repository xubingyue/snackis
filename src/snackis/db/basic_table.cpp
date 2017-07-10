#include "snackis/core/fmt.hpp"
#include "snackis/db/ctx.hpp"
#include "snackis/db/error.hpp"
#include "snackis/db/basic_table.hpp"

namespace snackis {  
namespace db {
  BasicTable::BasicTable(Ctx &ctx, const str &name): ctx(ctx), name(name) { }

  void open(BasicTable &tbl, std::ios_base::openmode mod) {
    tbl.file.open(get_path(tbl.ctx, tbl.name + ".tbl").string(),
		  std::ios::out | std::ios::binary | mod);
    if (tbl.file.fail()) {
      ERROR(Db, fmt("Failed opening file: %0", tbl.name));
    }
  }
}}
