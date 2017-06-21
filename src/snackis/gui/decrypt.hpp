#ifndef SNACKIS_GUI_DECRYPT_HPP
#define SNACKIS_GUI_DECRYPT_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Decrypt: View {
    GtkListStore *peers;
    GtkWidget *peer, *source, *target, *decode, *save, *cancel;
    
    Decrypt(Ctx &ctx);
    void focus() override;
  };
}}

#endif
