#include "snackis/ctx.hpp"
#include "snackis/setting.hpp"
#include "snackis/core/buf.hpp"
#include "snackis/db/table.hpp"

namespace snackis {
  Setting::Setting(Ctx &ctx, 
		   const str &key, const BasicType &type, 
		   opt<Val> init_val): 
    Rec(ctx), key(key), type(type) { }

  Setting::Setting(const db::Table<Setting> &tbl, const db::Rec<Setting> &rec):
    Rec(dynamic_cast<Ctx &>(tbl.ctx)), type(null_type) {
    copy(tbl, *this, rec);
  }

  opt<Val> get(Setting &setting) {
    if (setting.val.empty()) { load(setting.ctx.settings, setting); }

    if (setting.val.empty() && setting.init_val) { 
	set(setting, *setting.init_val); 
	return setting.init_val;
    }

    if (setting.val.empty()) { return none; }

    Buf buf;
    buf.write((char *)&setting.val[0], setting.val.size());
    return setting.type.read_val(buf);
  }

  void set(Setting &setting, const Val &val) {
    Buf buf;
    setting.type.write_val(val, buf);
    str data(buf.str());
    setting.val.assign(data.begin(), data.end());    
  }
}
