#ifndef SNACKIS_GUI_QUEUE_VIEW_HPP
#define SNACKIS_GUI_QUEUE_VIEW_HPP

#include "snackis/queue.hpp"
#include "snackis/gui/peer_list.hpp"
#include "snackis/gui/rec_view.hpp"

namespace snackis {
namespace gui {
  struct QueueView: RecView<Queue> {
    GtkWidget *find_tasks_btn, *new_task_btn, *find_posts_btn, *post_btn, *name_fld,
      *info_fld;
    PeerList peer_lst;
    
    QueueView(const Queue &rec);
    bool allow_save() const override;
    bool save() override;
  };
}}

#endif
