#include "snackis/ctx.hpp"
#include "snackis/core/fmt.hpp"
#include "snackis/ui/footer.hpp"
#include "snackis/ui/header.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/view_form.hpp"

namespace snackis {
namespace ui {
  ViewForm::ViewForm(View &view, Footer &ftr): Form(view, ftr), view(view) { }

  ViewForm::~ViewForm() {
    if (ptr) { pop_label(view.header); }
  }

  void open(ViewForm &frm) {
    open(dynamic_cast<Form &>(frm));
    push_label(frm.view.header, frm.label);
  }
}}
