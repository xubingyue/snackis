#ifndef SNACKIS_UI_READER_HPP
#define SNACKIS_UI_READER_HPP

#include <map>

#include "snackis/core/func.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"
#include "snackis/ui/form.hpp"
#include "snackis/ui/text_field.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  struct Footer;
  struct View;
  
  struct Reader: public Window {
    using Cmd = func<void ()>;
    
    Form form;
    TextField field;
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
}}

#endif
