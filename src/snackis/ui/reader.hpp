#ifndef SNACKIS_UI_READER_HPP
#define SNACKIS_UI_READER_HPP

#include <map>

#include "snackis/core/func.hpp"
#include "snackis/core/opt.hpp"
#include "snackis/core/str.hpp"
#include "snackis/ui/enum_field.hpp"
#include "snackis/ui/form.hpp"
#include "snackis/ui/window.hpp"

namespace snackis {
namespace ui {
  struct Footer;
  struct View;
  
  struct Reader: public Window {
    using Cmd = func<void ()>;
    
    Form form;
    EnumField<Cmd> field;
    opt<Cmd> last_cmd;
    bool quitting;
    View &view;
    
    Reader(Ctx &ctx, View &view, Footer &footer);
  };

  void run(Reader &rdr);
}}

#endif
