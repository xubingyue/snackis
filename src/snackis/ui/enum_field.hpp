#ifndef SNACKIS_UI_ENUM_FIELD_HPP
#define SNACKIS_UI_ENUM_FIELD_HPP

#include <map>
#include "snackis/ui/field.hpp"

namespace snackis {
namespace ui {
  using namespace snackis;

  template <typename T>
  struct EnumAlt {
    str lbl;
    T val;

    EnumAlt(const str &lbl, const T &val): lbl(lbl), val(val) { }
  };
  
  template <typename T>
  struct EnumField: public Field {
    std::vector<EnumAlt<T>> alts;
    std::map<str, size_t> lbl_lookup;
    std::map<T, size_t> alt_lookup;
    using OnSelect = func<void (opt<T> &val)>;
    opt<OnSelect> on_select;
    
    EnumField(Form &frm, const Dim &dim, const str &lbl);
    void drive(chtype ch) override;
  };

  template <typename T>
  EnumField<T>::EnumField(Form &frm, const Dim &dim, const str &lbl):
      Field(frm, dim, lbl) { }

  template <typename T>
  void EnumField<T>::drive(chtype ch) { }

  template <typename T>
  void push(EnumField<T> &fld, const str &lbl, const T &val) {
    const size_t i = fld.alts.size();
    fld.lbl_lookup[lbl] = i;
    fld.alt_lookup[val] = i;
    fld.alts.push_back(EnumAlt<T>(lbl, val));
  }

  template <typename T>
  bool select(EnumField<T> &fld, const T &val) {
    auto found(fld.alt_lookup.find(val));
    if (found == fld.alt_lookup.end()) { return false; } 
    const EnumAlt<T> alt(fld.alts[found->second]);
    set_str(fld, alt.lbl);
    return true;
  }
}}

#endif
