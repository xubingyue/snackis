#ifndef SNACKIS_GUI_CONSOLE_HPP
#define SNACKIS_GUI_CONSOLE_HPP

#include <vector>
#include <mutex>

#include <gtk/gtk.h>
#include "snackis/core/str.hpp"
#include "snackis/gui/widget.hpp"

namespace snackis {
namespace gui {
  struct Console: Widget {
    using LogLock = std::unique_lock<std::mutex>;
    GtkWidget *view;
    std::mutex log_mutex;
    std::vector<str> out;
    Console();
    GtkWidget *ptr() override;
  };

  void log(Console &cns, const str msg);
  void clear(Console &cns);
}}

#endif
