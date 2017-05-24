#ifndef SNACKIS_SETTING_HPP
#define SNACKIS_SETTING_HPP

#include "snackis/rec.hpp"
#include "snackis/core/data.hpp"
#include "snackis/core/optional.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/type.hpp"

namespace snackis {
  struct Setting: public Rec {
    str key;
    Data val;
    const BasicType &type;
    optional<Val> init_val;
    
    Setting(Ctx &ctx,
	    const str &key, const BasicType &type,
	    optional<Val> init_val);
    Setting(const db::Table<Setting> &tbl, const db::Rec<Setting> &rec);
  };

  optional<Val> get(Setting &setting);
  void set(Setting &setting, const Val &val);
}

#endif
