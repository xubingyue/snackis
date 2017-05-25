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
    case KEY_BACKSPACE:
      form_driver(frm.ptr, REQ_DEL_PREV);
      break;
    case KEY_DC:
      form_driver(frm.ptr, REQ_CLR_EOF);
      break;
    case KEY_HOME:
      form_driver(frm.ptr, REQ_BEG_FIELD);
      break;
    case KEY_END:
      eol(frm);
      break;
    case KEY_LEFT:
      form_driver(frm.ptr, REQ_PREV_CHAR);
      break;
    case KEY_RIGHT:
      form_driver(frm.ptr, REQ_NEXT_CHAR);
      break;
    case KEY_UP:
      form_driver(frm.ptr, REQ_PREV_FIELD);
      eol(frm);
      break;
    case KEY_RETURN:
      validate(frm);
    case KEY_DOWN:
      form_driver(frm.ptr, REQ_NEXT_FIELD);
      eol(frm);
      break;
    case KEY_TAB: {
      opt<Field &> fld(active_field(frm));

      if (fld && fld->complete) {
	validate(frm);
	str in(get_str(*fld)), out(fld->complete.get()(in));
	
	if (out != in) {
	  set_str(*fld, out);
	  eol(frm);
	}
      }

      break;
    }
   default:
      form_driver(frm.ptr, key);
    }
  }

  opt<Field &> active_field(Form &frm) {
    assert(frm.ptr);
    FIELD *ptr = current_field(frm.ptr);
    if (!ptr) { return none; }
    return *reinterpret_cast<Field *>(field_userptr(ptr));
  }
  
  void clear_field(Form &frm) {
    assert(frm.ptr);
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
    set_field_userptr(fld.ptr, reinterpret_cast<char *>(&fld));
    set_field_back(fld.ptr, A_UNDERLINE);
    field_opts_off(fld.ptr, O_AUTOSKIP);
  }

  void focus(Field &fld) {
    assert(fld.form.ptr);
    assert(fld.ptr);
    set_current_field(fld.form.ptr, fld.ptr);
  }

  str get_str(Field &fld) {
    assert(fld.ptr);
    return trim(str(field_buffer(fld.ptr, 0)));
  }

  void set_str(Field &fld, const str &val) {
    assert(fld.ptr);
    set_field_buffer(fld.ptr, 0, val.c_str());
  }
}
