#include <cassert>

#include "snackis/ctx.hpp"
#include "snackis/core/utils.hpp"
#include "snackis/ui/field.hpp"
#include "snackis/ui/form.hpp"
#include "snackis/ui/footer.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  void open(Form &frm) {
    assert(!frm.ptr);
    Pos pos(frm.margin_top, -1);
    for (auto f: frm.fields) {
      f->dim.w = min(f->dim.w+f->margin_left+f->margin_right,
		     frm.window.dim.w-frm.label_width);
      pos.y += f->margin_top;
      pos.x = frm.label_width + f->margin_left;
      open(*f, pos);
      pos.y += f->dim.h+1;
      frm.field_ptrs.push_back(f->ptr);
    }

    frm.field_ptrs.push_back(nullptr);
    frm.ptr = new_form(&frm.field_ptrs[0]);
    set_form_win(frm.ptr, frm.window.ptr);
    set_form_sub(frm.ptr, frm.window.ptr);
    post_form(frm.ptr);
    
    for (auto f: frm.fields) { paint(*f); }
    focus(frm);
  }

  void close(Form &frm) {
    assert(frm.ptr);
    unpost_form(frm.ptr);
    free_form(frm.ptr);
    frm.ptr = nullptr;
    refresh(frm.window);
  }

  void focus(Form &frm) {
    form_driver(frm.ptr, REQ_FIRST_FIELD);
    eol(frm);
    set_status(frm.footer, frm.status);
  }

  void eol(Form &frm) {
    form_driver(frm.ptr, REQ_END_FIELD);
    validate(frm);
  }
  
  void drive(Form &frm, chtype ch) {
    switch (ch) {
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
    case KEY_CTRL(KEY_SPACE): {
      Field &fld(active_field(frm));
      
      if (fld.on_action) { 
	validate(frm);
	(*fld.on_action)(); 
      }
      
      break;
    }
    default:
      drive(active_field(frm), ch);
    }
  }

  void validate(Form &frm) {
    form_driver(frm.ptr, REQ_VALIDATION);
  }

  Field &active_field(Form &frm) {
    assert(frm.ptr);
    FIELD *ptr(current_field(frm.ptr));
    assert(ptr);
    return *reinterpret_cast<Field *>(field_userptr(ptr));
  }
  
  void clear_field(Form &frm) {
    assert(frm.ptr);
    form_driver(frm.ptr, REQ_CLR_FIELD);
  }

  Form::Form(Window &wnd, Footer &ftr):
    window(wnd), footer(ftr), ctx(wnd.ctx), ptr(nullptr), label_width(0),
    margin_top(0) { }

  Form::~Form() {
    if (ptr) { close(*this); }
  }
}}
