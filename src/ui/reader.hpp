#ifndef SNACKIS_UI_READER_HPP
#define SNACKIS_UI_READER_HPP

#include <map>

#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"
#include "ui/form.hpp"
#include "ui/window.hpp"

namespace ui {
  struct Footer;
  struct View;
  
  struct Reader: public Window {
    using Cmd = std::function<void ()>;
    
    Form form;
    Field field;
    std::map<str, Cmd> cmds;
    opt<Cmd> last_cmd;
    bool quitting;
    View &view;
    
    Reader(Ctx &ctx, View &view, Footer &footer);
  };

  void init_cmds(Reader &rdr);
  bool run_cmd(Reader &rdr, const str &in);
  void run_once(Reader &rdr);
  void run(Reader &rdr);
}

#endif
