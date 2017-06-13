#ifndef SNACKIS_GUI_SETUP_HPP
#define SNACKIS_GUI_SETUP_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Setup: public View {
    GtkWidget *name, *email, *load_folder, *save_folder, *cancel, *save;
    
    Setup(Ctx &ctx);
    void focus() override;
  };
}}

#endif
