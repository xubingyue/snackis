#include <cassert>

#include "snackis/core/utils.hpp"
#include "snackis/ui/field.hpp"
#include "snackis/ui/form.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  void set_bg(Field &fld, chtype ch) {
    set_field_back(fld.ptr, ch);
  }

  void open(Field &fld, const Pos &pos) { fld.open(pos); }

  void paint(Field &fld) { fld.paint(); }

  void focus(Field &fld) {
    assert(fld.form.ptr);
    assert(fld.ptr);
    set_current_field(fld.form.ptr, fld.ptr);
  }

  void drive(Field &fld, chtype ch) {
    fld.drive(ch);
  }

  str get_str(Field &fld) {
    assert(fld.ptr);
    return trim(str(field_buffer(fld.ptr, 0)));
  }

  void set_str(Field &fld, const str &val) {
    assert(fld.ptr);
    set_field_buffer(fld.ptr, 0, val.c_str());
  }

  Field::Field(Form &frm, const Dim &dim, const str &lbl):
    form(frm), 
    pos(-1, -1),
    dim(dim),
    margin_left(0), margin_right(0), margin_top(0), 
    label(lbl), ptr(nullptr), 
    active(true) {
    frm.fields.push_back(this);
    frm.label_width = max(frm.label_width, lbl.size());
  }

  Field::~Field() { free_field(ptr); }

  void Field::paint() {
    Window &wnd(form.window);
    move(wnd, Pos(pos.y, pos.x-margin_left-label.size()));
    print(wnd, label);
  }

  void Field::open(const Pos &pos) {
    assert(!ptr);
    ptr = new_field(dim.h, dim.w, pos.y, pos.x, 0, 0);
    set_field_userptr(ptr, reinterpret_cast<char *>(this));
    field_opts_off(ptr, O_AUTOSKIP);
    if (active) { set_bg(*this, A_UNDERLINE); }
    else { field_opts_off(ptr, O_ACTIVE); }
    this->pos = pos;
  }
}}
