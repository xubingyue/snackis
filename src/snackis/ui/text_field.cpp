#include "snackis/ui/form.hpp"
#include "snackis/ui/text_field.hpp"

namespace snackis {
namespace ui {
  TextField::TextField(Form &frm, const Dim &dim, const str &lbl):
    Field(frm, dim, lbl), echo(true) { }

  void TextField::show(const Pos &pos) {
    Field::show(pos);
    if (!echo) { field_opts_off(ptr, O_PUBLIC); }
  }

  void TextField::drive(chtype ch) {
    switch (ch) {
    case KEY_BACKSPACE:
      form_driver(form.ptr, REQ_DEL_PREV);
      break;
    case KEY_DC:
      form_driver(form.ptr, REQ_CLR_EOF);
      break;
    case KEY_END:
      eol(form);
      break;
    case KEY_LEFT:
      form_driver(form.ptr, REQ_PREV_CHAR);
      break;
    case KEY_RIGHT:
      form_driver(form.ptr, REQ_NEXT_CHAR);
      break;
    case KEY_HOME:
      form_driver(form.ptr, REQ_BEG_FIELD);
      break;
    case KEY_TAB: {
      if (on_complete) {
	validate(form);
	str in(get_str(*this)), out((*on_complete)(in));
      
	if (out != in) {
	  set_str(*this, out);
	  eol(form);
	}
      }
    
      break;
    }    
    default:
      form_driver(form.ptr, ch);
    };
  }
}}
