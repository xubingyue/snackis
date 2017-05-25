#ifndef SNACKIS_UI_FORM_HPP
#define SNACKIS_UI_FORM_HPP

#include <vector>

#include <form.h>

#include "snackis/core/str.hpp"
#include "ui/ui.hpp"

namespace ui {
  using namespace snackis;
  
  struct Window;
  struct Field;

  struct Form {
    Window &window;
    FORM *ptr;
    std::vector<Field *> fields;
    std::vector<FIELD *> field_ptrs;
    int label_width;
    Form(Window &wnd);
    virtual ~Form();
  };

  struct Field {
    using Complete = std::function<str (const str &)>;

    Form &form;
    Dim dim;
    str label;
    FIELD *ptr;
    opt<Complete> complete;
    
    Field(Form &frm, const Dim &dim, const str &lbl);
    virtual ~Field();
  };

  void open(Form &frm);
  void close(Form &frxm);
  void focus(Form &frm);
  void eol(Form &frm);
  void validate(Form &frm);
  void drive(Form &frm, int key);
  Field &active_field(Form &frm);
  void clear_field(Form &frm);

  void set_bg(Field &fld, chtype ch);
  void show(Field &fld, const Pos &pos);
  void focus(Field &fld);
  str get_str(Field &fld);
  void set_str(Field &fld, const str &val);  
}

#endif
