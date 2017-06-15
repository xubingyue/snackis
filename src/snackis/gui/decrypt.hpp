#ifndef SNACKIS_GUI_DECRYPT_HPP
#define SNACKIS_GUI_DECRYPT_HPP

#include <vector>
#include <mutex>

#include "snackis/gui/view.hpp"

namespace snackis {
namespace gui {
  struct Decrypt: public View {
    GtkListStore *peers;
    GtkWidget *peer, *source, *target, *decode, *cancel, *save;
    
    Decrypt(Ctx &ctx);
    void focus() override;
  };
}}

#endif
