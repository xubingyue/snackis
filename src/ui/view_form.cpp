#include "snackis/core/defer.hpp"

#include "ui/footer.hpp"
#include "ui/header.hpp"
#include "ui/view.hpp"
#include "ui/view_form.hpp"

namespace ui {
  ViewForm::ViewForm(View &view, Footer &ftr): Form(view, ftr), view(view) { }

  void open(ViewForm &frm) {
    open(dynamic_cast<Form &>(frm));
    set_label(frm.view.header, frm.label);    
  }
}
