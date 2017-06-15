#ifndef SNACKIS_GUI_ENCRYPT_HPP
#define SNACKIS_GUI_ENCRYPT_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Encrypt: public View {
    GtkListStore *peers;
    GtkWidget *peer, *source, *target, *encode, *cancel, *save;
    
    Encrypt(Ctx &ctx);
    void focus() override;
  };
}}

#endif
