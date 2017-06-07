#ifndef SNACKIS_UI_FIELD_HPP
#define SNACKIS_UI_FIELD_HPP

#include <vector>

#include <form.h>

#include "snackis/core/func.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"
#include "snackis/ui/ui.hpp"

namespace snackis {
namespace ui {
  using namespace snackis;

  struct Form;
  
  struct Field {
    using OnAction = func<void ()>;
    
    Form &form;
    Pos pos;
    Dim dim;
    int margin_top;
    str label, info;
    char symbol;
    FIELD *ptr;
    bool active;
    opt<OnAction> on_action;
    
    Field(Form &frm, const Dim &dim, const str &lbl);
    virtual ~Field();
    virtual void drive(chtype ch) = 0;
    virtual void open(const Pos &pos);
    virtual void on_focus();
    virtual void paint();
  };

  void set_bg(Field &fld, chtype ch);
  size_t label_width(const Field &fld);

  void open(Field &fld, const Pos &pos);
  void on_focus(Field &fld);
  void paint(Field &fld);
  void focus(Field &fld);
  void drive(Field &fld, chtype ch);
  str get_str(Field &fld);
  void set_str(Field &fld, const str &val);  
}}

#endif
