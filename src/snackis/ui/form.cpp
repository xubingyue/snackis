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

    size_t max_width = 0;
    for (auto f: frm.fields) {
      max_width = max(max_width, label_width(*f));
    }
    
    Pos pos(frm.margin_top, max_width);
    for (auto f: frm.fields) {
      f->dim.w = min(f->dim.w, frm.window.dim.w-max_width);
      pos.y += f->margin_top;
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
    on_focus(active_field(frm));
    set_status(frm.footer, frm.status);
  }

  void eol(Form &frm) {
    form_driver(frm.ptr, REQ_END_FIELD);
    validate(frm);
  }
  
  void drive(Form &frm, chtype ch) {
    Field &f(active_field(frm));

    switch (ch) {
    case KEY_UP:
      if (f.dim.h > 1) {
	drive(f, ch);
	break;
      }
      
      form_driver(frm.ptr, REQ_PREV_FIELD);
      on_focus(f);
      break;
    case KEY_RETURN:
      validate(frm);
      form_driver(frm.ptr, REQ_NEXT_FIELD);
      on_focus(active_field(frm));
      break;
    case KEY_DOWN:
      if (f.dim.h > 1) {
	drive(f, ch);
	break;
      }

      form_driver(frm.ptr, REQ_NEXT_FIELD);
      on_focus(active_field(frm));
      break;
    case KEY_CTRL(KEY_SPACE): {
      if (f.on_action) { 
	validate(frm);
	(*f.on_action)(); 
      }
      
      break;
    }
    default:
      drive(f, ch);
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
    window(wnd), footer(ftr), ctx(wnd.ctx), ptr(nullptr), margin_top(0) { }

  Form::~Form() {
    if (ptr) { close(*this); }
  }
}}
