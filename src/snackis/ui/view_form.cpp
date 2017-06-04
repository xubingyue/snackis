#include "snackis/core/defer.hpp"
#include "snackis/ui/footer.hpp"
#include "snackis/ui/header.hpp"
#include "snackis/ui/view.hpp"
#include "snackis/ui/view_form.hpp"

namespace snackis {
namespace ui {
  ViewForm::ViewForm(View &view, Footer &ftr): Form(view, ftr), view(view) { }

  void open(ViewForm &frm) {
    open(dynamic_cast<Form &>(frm));
    set_label(frm.view.header, frm.label);    
  }
}}
