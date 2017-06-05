#ifndef SNACKIS_UI_FORM_HPP
#define SNACKIS_UI_FORM_HPP

#include <vector>

#include <form.h>

#include "snackis/core/func.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"
#include "snackis/ui/ui.hpp"

namespace snackis {
  struct Ctx;
  
namespace ui {
  using namespace snackis;
  
  struct Window;
  struct Field;
  struct Footer;
  
  struct Form {
    Window &window;
    Footer &footer;
    Ctx &ctx;
    FORM *ptr;
    std::vector<Field *> fields;
    std::vector<FIELD *> field_ptrs;
    int label_width, margin_top;
    str label, status;
    Form(Window &wnd, Footer &ftr);
    virtual ~Form();
  };

  void open(Form &frm);
  void close(Form &frxm);
  void focus(Form &frm);
  void eol(Form &frm);
  void drive(Form &frm, chtype ch);
  void validate(Form &frm);
  Field &active_field(Form &frm);
  void clear_field(Form &frm);
}}

#endif
