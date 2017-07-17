#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/task_search.hpp"
#include "snackis/gui/task_view.hpp"

namespace snackis {
namespace gui {
  enum TaskCol {COL_PTR=0, COL_ID, COL_CREATED, COL_OWNER, COL_PROJECT, COL_PRIO,
		COL_TAGS, COL_INFO};

  static void edit(Ctx &ctx, const db::Rec<Task> &rec) {
    Task task(ctx, rec);
    push_view(new TaskView(task));
  }

  TaskSearch::TaskSearch(Ctx &ctx):
    SearchView<Task>(ctx,
		     "Task",
		     gtk_list_store_new(8, G_TYPE_POINTER,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING),
		     [&ctx](auto &rec) { edit(ctx, rec); }),
    id_fld(new_id_field()),
    prio_fld(gtk_entry_new()),
    done_fld(gtk_check_button_new_with_label("Done")),
    tags_fld(gtk_entry_new()),
    text_fld(gtk_entry_new()),
    project_fld(ctx),
    peer_fld(ctx)
  {
    GtkWidget *frm(new_grid());
    gtk_grid_set_column_homogeneous(GTK_GRID(frm), true);
    gtk_container_add(GTK_CONTAINER(fields), frm);
    int row = 0;
    
    gtk_grid_attach(GTK_GRID(frm), new_label("Id"), 0, row, 2, 1);
    gtk_widget_set_halign(id_fld, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(frm), id_fld, 0, row+1, 2, 1);

    gtk_grid_attach(GTK_GRID(frm), new_label("Prio"), 2, 0, 1, 1);
    set_width(GTK_ENTRY(prio_fld), 3);
    gtk_widget_set_halign(prio_fld, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(frm), prio_fld, 2, 1, 1, 1);		    
    
    gtk_widget_set_halign(done_fld, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(frm), done_fld, 3, row+1, 1, 1);

    row += 2;
    gtk_grid_attach(GTK_GRID(frm), new_label("Tags"), 0, row, 2, 1);
    gtk_widget_set_hexpand(tags_fld, true);
    gtk_grid_attach(GTK_GRID(frm), tags_fld, 0, row+1, 2, 1);

    gtk_grid_attach(GTK_GRID(frm), new_label("Text"), 2, row, 2, 1);
    gtk_widget_set_hexpand(text_fld, true);
    gtk_grid_attach(GTK_GRID(frm), text_fld, 2, row+1, 2, 1);

    row += 2;
    gtk_grid_attach(GTK_GRID(frm), new_label("Project"), 0, row, 2, 1);
    gtk_grid_attach(GTK_GRID(frm), project_fld.ptr(), 0, row+1, 2, 1);
    
    gtk_grid_attach(GTK_GRID(frm), new_label("Owner"), 2, row, 2, 1);
    gtk_grid_attach(GTK_GRID(frm), peer_fld.ptr(), 2, row+1, 2, 1);

    add_col(GTK_TREE_VIEW(list), "Id", COL_ID);
    add_col(GTK_TREE_VIEW(list), "Created", COL_CREATED);
    add_col(GTK_TREE_VIEW(list), "Owner", COL_OWNER);
    add_col(GTK_TREE_VIEW(list), "Project", COL_PROJECT);
    add_col(GTK_TREE_VIEW(list), "Prio", COL_PRIO);
    add_col(GTK_TREE_VIEW(list), "Tags", COL_TAGS);
    add_col(GTK_TREE_VIEW(list), "Info", COL_INFO, true);
    focused = id_fld;
  }

  void TaskSearch::find() {
    size_t cnt(0);    

    str id_sel(get_str(GTK_ENTRY(id_fld)));
    str prio_str(get_str(GTK_ENTRY(prio_fld)));
    int64_t prio_sel(to_int64(prio_str));
    bool done_sel(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(done_fld)));
    str tags_str(get_str(GTK_ENTRY(tags_fld)));
    std::set<str> tags_sel(word_set(tags_str));
    str text_sel(get_str(GTK_ENTRY(text_fld)));
    auto peer_sel(peer_fld.selected);
    
    for (auto i = ctx.db.tasks_sort.recs.begin();
	 i != ctx.db.tasks_sort.recs.end();
	 i++) {
      auto &rec(db::get(ctx.db.tasks, *i));
      Task tsk(ctx, rec);
      
      if (!id_sel.empty() && find_ci(id_str(tsk), id_sel) == str::npos) { continue; }

      if (!tags_sel.empty()) {
	std::vector<str> diff;
	std::set_difference(tags_sel.begin(), tags_sel.end(),
			    tsk.tags.begin(), tsk.tags.end(),
			    std::back_inserter(diff));
	if (!diff.empty()) { continue; }
      }
      
      if (!text_sel.empty() &&
	  find_ci(tsk.name, text_sel) == str::npos &&
	  find_ci(tsk.info, text_sel) == str::npos) { continue; }
      
      if (!prio_str.empty() && prio_sel && tsk.prio > prio_sel) { continue; }

      if (tsk.done != done_sel) { continue; }
      
      auto project_sel(project_fld.selected);
      if (project_sel && tsk.project_id != project_sel->id) { continue; }
      
      if (peer_sel && tsk.owner_id != peer_sel->id) { continue; }
      
      Project prj(get_project_id(ctx, tsk.project_id));
      Peer own(get_peer_id(ctx, tsk.owner_id));
      GtkTreeIter iter;
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter,
			 COL_PTR, &rec,
			 COL_ID, id_str(tsk).c_str(),
			 COL_CREATED,
			 fmt(tsk.created_at, "%a %b %d, %H:%M").c_str(),
			 COL_PROJECT, id_str(prj).c_str(),
			 COL_OWNER, own.name.c_str(),
			 COL_PRIO, to_str(tsk.prio).c_str(),
			 COL_TAGS,
			 join(tsk.tags.begin(), tsk.tags.end(), '\n').c_str(),
			 COL_INFO, trim(fmt("%0\n%1", tsk.name, tsk.info)).c_str(),
			 -1);
      
      cnt++;
    }
    
    gtk_widget_grab_focus(cnt ? list : id_fld);
  }
}}
