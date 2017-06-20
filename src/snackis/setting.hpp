#ifndef SNACKIS_SETTING_HPP
#define SNACKIS_SETTING_HPP

#include "snackis/rec.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/stream.hpp"
#include "snackis/core/type.hpp"

namespace snackis {
  struct BasicSetting: Rec {
    str key, val;

    BasicSetting(Ctx &ctx, const str &key);
    BasicSetting(Ctx &ctx, const db::Rec<BasicSetting> &rec);
  };

  template <typename ValT>
  struct Setting: BasicSetting {
    const Type<ValT> &type;
    opt<ValT> init_val;

    Setting(Ctx &ctx,
	    const str &key, const Type<ValT> &type,
	    opt<ValT> init_val);
  };

  template <typename ValT>
  Setting<ValT>::Setting(Ctx &ctx,
			 const str &key, const Type<ValT> &type, 
			 opt<ValT> init_val): 
    BasicSetting(ctx, key), type(type), init_val(init_val) { }
  
  template <typename ValT>
  opt<ValT> get_val(Setting<ValT> &stn) {
    if (stn.val.empty()) {
      load(stn.ctx.db.settings, dynamic_cast<BasicSetting &>(stn));
    }

    if (stn.val.empty()) { return stn.init_val; }
    Stream buf(stn.val);
    return stn.type.read(buf);
  }
  
  template <typename ValT>
  void set_val(Setting<ValT> &stn, const ValT &val) {
    Stream buf;
    stn.type.write(val, buf);
    stn.val = buf.str();
    upsert(stn.ctx.db.settings, dynamic_cast<BasicSetting &>(stn));
  }
}

#endif
