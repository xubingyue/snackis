#ifndef SNACKIS_GUI_LOGIN_HPP
#define SNACKIS_GUI_LOGIN_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Login: public View {
    GtkWidget *pass;
    
    Login(Ctx &ctx);
    void focus() override;
  };
}}

#endif
