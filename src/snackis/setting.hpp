#ifndef SNACKIS_SETTING_HPP
#define SNACKIS_SETTING_HPP

#include "snackis/rec.hpp"
#include "snackis/core/buf.hpp"
#include "snackis/core/data.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/type.hpp"

namespace snackis {
  struct BasicSetting: public Rec {
    str key;
    Data val;
    const BasicType &type;

    BasicSetting(Ctx &ctx, const str &key, const BasicType &type);
    BasicSetting(const db::Table<BasicSetting> &tbl,
		 const db::Rec<BasicSetting> &rec);
  };

  template <typename ValT>
  struct Setting: public BasicSetting {
    opt<ValT> init_val;

    Setting(Ctx &ctx,
	    const str &key, const Type<ValT> &type,
	    opt<ValT> init_val);
  };

  template <typename ValT>
  Setting<ValT>::Setting(Ctx &ctx,
			 const str &key, const Type<ValT> &type, 
			 opt<ValT> init_val): 
    BasicSetting(ctx, key, type), init_val(init_val) { }
  
  template <typename ValT>
  opt<ValT> get(Setting<ValT> &stn) {
    if (stn.val.empty()) { load(stn.ctx.db.settings, stn); }

    if (stn.val.empty() && stn.init_val) { 
	set_val(stn, *stn.init_val); 
	return stn.init_val;
    }

    if (stn.val.empty()) { return none; }

    Buf buf;
    buf.write((char *)&stn.val[0], stn.val.size());
    return stn.type.read_val(buf);
  }
  
  template <typename ValT>
  void set_val(Setting<ValT> &stn, const ValT &val) {
    Buf buf;
    stn.type.write_val(val, buf);
    str data(buf.str());
    stn.val.assign(data.begin(), data.end());
    upsert(stn.ctx.db.settings, stn);
  }
}

#endif
