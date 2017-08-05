#ifndef SNACKIS_GUI_SCRIPT_VIEW_HPP
#define SNACKIS_GUI_SCRIPT_VIEW_HPP

#include "snabel/exec.hpp"
#include "snackis/script.hpp"
#include "snackis/gui/feed_history.hpp"
#include "snackis/gui/peer_list.hpp"
#include "snackis/gui/shared_view.hpp"

namespace snackis {
namespace gui {
  struct ScriptView: SharedView<Script> {
    GtkListStore *bcode_store;

    GtkWidget *new_script_btn, *find_posts_btn, *post_btn, *name_fld,
      *tags_fld, *code_fld, *bcode_lst, *compile_btn, *run_btn;
    
    PeerList peer_lst;
    FeedHistory post_lst;
    snabel::Exec exec;
    
    ScriptView(const Script &rec);
    bool save() override;
  };
}}

#endif
