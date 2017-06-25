#ifndef SNACKIS_GUI_READER_HPP
#define SNACKIS_GUI_READER_HPP

#include <map>
#include <gtk/gtk.h>
#include "snackis/core/func.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  struct Reader: Widget {
    using Cmd = func<bool (const std::vector<str> &)>;

    Ctx &ctx;
    GtkWidget *entry;
    std::map<str, Cmd> cmds;
    opt<str> last_cmd;
    
    Reader(Ctx &ctx);
    GtkWidget *ptr() override;
  };
}}

#endif
