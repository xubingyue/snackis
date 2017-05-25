#include <cassert>

#include "snackis/ctx.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/core/utils.hpp"
#include "ui/form.hpp"
#include "ui/window.hpp"

namespace ui {
  Form::Form(Window &wnd): window(wnd), ptr(nullptr), label_width(0) { }

  Form::~Form() {
    if (ptr) { close(*this); }
  }
  
  void open(Form &frm) {
    assert(!frm.ptr);
    Pos pos(0, frm.label_width);

    for (auto f: frm.fields) {
      f->dim.w = min(f->dim.w, frm.window.dim.w-frm.label_width);
      show(*f, pos);
      pos.y += f->dim.h+1;
      frm.field_ptrs.push_back(f->ptr);
    }

    frm.field_ptrs.push_back(nullptr);
    frm.ptr = new_form(&frm.field_ptrs[0]);
    set_form_win(frm.ptr, frm.window.ptr);
    set_form_sub(frm.ptr, frm.window.ptr);
    post_form(frm.ptr);
  }

  void close(Form &frm) {
    assert(frm.ptr);
    unpost_form(frm.ptr);
    free_form(frm.ptr);
    frm.ptr = nullptr;
  }

  void focus(Form &frm) {
    form_driver(frm.ptr, REQ_FIRST_FIELD);
    eol(frm);
  }

  void eol(Form &frm) {
    form_driver(frm.ptr, REQ_END_FIELD);
    validate(frm);
  }

  void validate(Form &frm) {
    form_driver(frm.ptr, REQ_VALIDATION);
  }
  
  void drive(Form &frm, int key) {
    switch (key) {
    case KEY_RETURN:
      validate(frm);
      break;
    default:
      form_driver(frm.ptr, key);
    }
  }

  Field &active_field(Form &frm) {
    FIELD *ptr = current_field(frm.ptr);
    return *reinterpret_cast<Field *>(field_userptr(ptr));
  }
  
  void clear_field(Form &frm) {
    form_driver(frm.ptr, REQ_CLR_FIELD);
  }
  
  Field::Field(Form &frm, const Dim &dim, const str &lbl):
    form(frm), dim(dim), label(lbl) {
    frm.fields.push_back(this);
    frm.label_width = max(frm.label_width, lbl.size());
  }

  Field::~Field() {
    free_field(ptr);
  }

  void show(Field &fld, const Pos &pos) {
    assert(!fld.ptr);
    fld.ptr = new_field(fld.dim.h, fld.dim.w, pos.y, pos.x, 0, 0);
    set_field_userptr(fld.ptr, reinterpret_cast<char *>(fld.ptr));
    set_field_back(fld.ptr, A_UNDERLINE);
    field_opts_off(fld.ptr, O_AUTOSKIP);
  }

  str get_str(Field &fld) {
    return trim(str(field_buffer(fld.ptr, 0)));
  }
}
