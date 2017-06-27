#ifndef SNACKIS_GUI_ENCRYPT_HPP
#define SNACKIS_GUI_ENCRYPT_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Encrypt: View {
    GtkListStore *peers;
    GtkWidget *peer, *source, *target, *encode, *save, *cancel;
    
    Encrypt(Ctx &ctx);
    void init() override;
  };
}}

#endif
