#ifndef SNACKIS_UI_ENUM_FIELD_HPP
#define SNACKIS_UI_ENUM_FIELD_HPP

#include <map>
#include "snackis/ui/field.hpp"

namespace snackis {
namespace ui {
  using namespace snackis;

  template <typename T>
  struct EnumField: public Field {
    std::map<str, T> alts;
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
}}

#endif
