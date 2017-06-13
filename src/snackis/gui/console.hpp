#ifndef SNACKIS_GUI_CONSOLE_HPP
#define SNACKIS_GUI_CONSOLE_HPP

#include <vector>
#include <mutex>

#include <gtk/gtk.h>
#include "snackis/core/fmt.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"
#include "snackis/core/time.hpp"

namespace snackis {
namespace gui {
  struct Console {
    using LogLock = std::unique_lock<std::mutex>;
    GtkWidget *text_view, *scroll_view;
    std::mutex log_mutex;
    std::vector<str> out;
    Console();
  };

  GtkWidget *widget(Console &console);

  template <typename...Args>
  void log(Console &cns, const str &spec, Args...args) {
    const str msg(fmt("%0 %1\n", 
		      fmt(now(), "%a %H:%M:%S"), 
		      fmt(spec, std::forward<Args>(args)...)));
    Console::LogLock lock(cns.log_mutex);
    cns.out.push_back(msg);
  }

  void refresh(Console &cns);
}}

#endif
