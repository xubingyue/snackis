#ifndef SNACKIS_UI_ENUM_FIELD_HPP
#define SNACKIS_UI_ENUM_FIELD_HPP

#include <cctype>
#include <map>
#include "snackis/ui/field.hpp"
#include "snackis/ui/window.hpp"

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
    using OnSelect = func<void (const opt<T> &val)>;
    opt<OnSelect> on_select;
    str search;
    bool allow_clear;
    
    EnumField(Form &frm, const Dim &dim, const str &lbl);
    void on_focus() override;
    void drive(chtype ch) override;
  };

  template <typename T>
  EnumField<T>::EnumField(Form &frm, const Dim &dim, const str &lbl):
    Field(frm, dim, lbl), allow_clear(false) {
    symbol = '=';
  }

  template <typename T>
  void EnumField<T>::on_focus() { search.clear(); }

  template <typename T>
  void EnumField<T>::drive(chtype ch) {
    switch (ch) {
    case KEY_SPACE:
      if (!alts.empty()) {
	auto found = lbl_lookup.find(get_str(*this));
	if (found != lbl_lookup.end()) { found++; }
	if (found == lbl_lookup.end()) { found = lbl_lookup.begin(); }
	const EnumAlt<T> alt(alts[found->second]);
	select(*this, alt.val);
      }
      
      break;
    case KEY_BACKSPACE:
      if (!search.empty()) {
	search.pop_back();
	form_driver(form.ptr, REQ_PREV_CHAR);
      }
      break;
    case KEY_DC:
      if (allow_clear) { clear(*this); }
      search.clear();
      break;
    default:
      if (std::isgraph(ch)) {
	str nsearch(search);
	nsearch.push_back(ch);
	auto found(lbl_lookup.lower_bound(nsearch));
	if (found != lbl_lookup.end() && found->first.find(nsearch) != str::npos) {
	  search.push_back(ch);
	  const EnumAlt<T> alt(alts[found->second]);
	  select(*this, alt.val);
	  
	  for (int i=0; i < search.size(); i++) {
	    form_driver(form.ptr, REQ_NEXT_CHAR);
	  }
	}
      }
    }
  }

  template <typename T>
  void push(EnumField<T> &fld, const str &lbl, const T &val) {
    const size_t i = fld.alts.size();
    fld.lbl_lookup[lbl] = i;
    fld.alt_lookup[val] = i;
    fld.alts.push_back(EnumAlt<T>(lbl, val));
  }

  template <typename T>
  bool select(EnumField<T> &fld, const T &val, bool trig = true) {
    auto found(fld.alt_lookup.find(val));
    if (found == fld.alt_lookup.end()) { return false; } 
    const EnumAlt<T> alt(fld.alts[found->second]);
    set_str(fld, alt.lbl);
    if (trig && fld.on_select) { (*fld.on_select)(val); }
    return true;
  }

  template <typename T>
  void clear(EnumField<T> &fld, bool trig = true) {
      set_str(fld, "");
      if (trig && fld.on_select) { (*fld.on_select)(nullopt); }
  }
}}

#endif
