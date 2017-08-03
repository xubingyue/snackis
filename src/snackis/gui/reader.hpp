#ifndef SNACKIS_GUI_READER_HPP
#define SNACKIS_GUI_READER_HPP

#include <set>
#include <gtk/gtk.h>
#include "snabel/exec.hpp"
#include "snackis/core/func.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  struct Reader: Widget {
    using Cmd = func<void (const snabel::Args &)>;

    Ctx &ctx;
    GtkWidget *entry;
    std::set<str> cmds;
    opt<str> last_cmd;
    snabel::Exec exec;
    
    Reader(Ctx &ctx);
    GtkWidget *ptr() override;
  };
}}

#endif
