#ifndef SNACKIS_UI_TEXT_FIELD_HPP
#define SNACKIS_UI_TEXT_FIELD_HPP

#include <map>
#include "snackis/ui/field.hpp"

namespace snackis {
namespace ui {
  using namespace snackis;

  struct TextField: public Field {
    bool echo;
    using OnComplete = func<str (const str &)>;
    opt<OnComplete> on_complete;
    
    TextField(Form &frm, const Dim &dim, const str &lbl);
    void show(const Pos &pos) override;
    void drive(chtype ch) override;
  };
}}

#endif
