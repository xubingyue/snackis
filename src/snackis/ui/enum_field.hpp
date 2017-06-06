#ifndef SNACKIS_UI_ENUM_FIELD_HPP
#define SNACKIS_UI_ENUM_FIELD_HPP

#include <cctype>
#include <map>
#include <unordered_map>

#include "snackis/ui/field.hpp"
#include "snackis/ui/form.hpp"
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
    std::map<str, EnumAlt<T>> alts;
    opt<EnumAlt<T>> selected;
    using OnSelect = func<void ()>;
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
	auto found = alts.find(get_str(*this));
	if (found != alts.end()) { found++; }
	if (found == alts.end()) { found = alts.begin(); }
	select(*this, found->second);
      }
      
      break;
    case KEY_BACKSPACE:
      if (!search.empty()) {
	search.pop_back();
	form_driver(form.ptr, REQ_PREV_CHAR);
      }

      if (allow_clear && search.empty()) {
	clear(*this);
      }

      break;
    case KEY_DC:
      if (allow_clear) {
	selected = nullopt;
	clear(*this);
      }
      
      search.clear();
      break;
    default:
      if (std::isgraph(ch)) {
	str nsearch(search);
	nsearch.push_back(ch);
	auto found(alts.lower_bound(nsearch));
	if (found != alts.end() && found->first.find(nsearch) != str::npos) {
	  search.push_back(ch);
	  select(*this, found->second);
	  
	  for (int i=0; i < search.size(); i++) {
	    form_driver(form.ptr, REQ_NEXT_CHAR);
	  }
	}
      }
    }
  }

  template <typename T>
  void insert(EnumField<T> &fld, const str &lbl, const T &val) {
    fld.alts.emplace(lbl, EnumAlt<T>(lbl, val));
  }

  template <typename T>
  void select(EnumField<T> &fld, const EnumAlt<T> &alt, bool trig = true) {
    fld.selected = alt;
    set_str(fld, alt.lbl);    
    if (trig && fld.on_select) { (*fld.on_select)(); }
  }

  template <typename T>
  bool select(EnumField<T> &fld, const T &val, bool trig = true) {
    auto found(std::find_if(fld.alts.begin(), fld.alts.end(),
			    [&val](auto &a) { return a.second.val == val; }));
    if (found == fld.alts.end()) { return false; }
    select(fld, found->second, trig);
    return true;
  }

  template <typename T>
  void clear(EnumField<T> &fld, bool trig = true) {
      set_str(fld, "");
      fld.selected = nullopt;
      if (trig && fld.on_select) { (*fld.on_select)(); }
  }
}}

#endif
