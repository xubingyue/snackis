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
    Form &form;
    Dim dim;
    str label;
    FIELD *ptr;
    
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

  void show(Field &fld, const Pos &pos);
  str get_str(Field &fld);
}

#endif
