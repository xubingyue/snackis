#ifndef SNACKIS_UI_VIEW_FORM_HPP
#define SNACKIS_UI_VIEW_FORM_HPP

#include <memory>

#include "snackis/ui/form.hpp"

namespace snackis {
namespace ui {
  struct View;
  
  struct ViewForm: public Form {
    View &view;
    
    ViewForm(View &view, Footer &ftr);
  };
}}

#endif
