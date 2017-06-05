#include <cassert>

#include "snackis/core/utils.hpp"
#include "snackis/ui/field.hpp"
#include "snackis/ui/form.hpp"

namespace snackis {
namespace ui {
  void set_bg(Field &fld, chtype ch) {
    set_field_back(fld.ptr, ch);
  }

  void show(Field &fld, const Pos &pos) {
    assert(!fld.ptr);
    fld.ptr = new_field(fld.dim.h, fld.dim.w, pos.y, pos.x, 0, 0);
    set_field_userptr(fld.ptr, reinterpret_cast<char *>(&fld));
    field_opts_off(fld.ptr, O_AUTOSKIP);
    if (fld.active) { set_bg(fld, A_UNDERLINE); }
    else { field_opts_off(fld.ptr, O_ACTIVE); }
    if (!fld.echo) { field_opts_off(fld.ptr, O_PUBLIC); }
  }

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
    dim(dim), margin_top(0), 
    label(lbl), ptr(nullptr), 
    active(true), echo(true) {
    frm.fields.push_back(this);
    frm.label_width = max(frm.label_width, lbl.size());
  }

  Field::~Field() { free_field(ptr); }

  void Field::drive(chtype ch) {
    form_driver(form.ptr, ch);
  }
}}
