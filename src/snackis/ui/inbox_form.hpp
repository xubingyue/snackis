#ifndef SNACKIS_UI_INBOX_FORM_HPP
#define SNACKIS_UI_INBOX_FORM_HPP

#include "snackis/ui/view_form.hpp"
#include "snackis/ui/field.hpp"

namespace snackis {
namespace ui {
  struct InboxForm: public ViewForm {
    std::map<UId, Field *> field_lookup;
    InboxForm(View &view, Footer &ftr);
  };

  bool run(InboxForm &frm);
}}

#endif
