#ifndef SNACKIS_GUI_LOGIN_HPP
#define SNACKIS_GUI_LOGIN_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Login: View {
    GtkWidget *pass, *repeat, *btn;
    
    Login(Ctx &ctx);
    void init() override;
  };
}}

#endif
