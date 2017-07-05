#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/project_search.hpp"
#include "snackis/gui/project_view.hpp"

namespace snackis {
namespace gui {
  enum ProjectCol {COL_PTR=0, COL_ID, COL_CREATED, COL_OWNER, COL_NAME};

  static void edit(Ctx &ctx, const db::Rec<Project> &rec) {
    ProjectView *fv(new ProjectView(Project(ctx, rec)));
    push_view(*fv);
  }
  
  ProjectSearch::ProjectSearch(Ctx &ctx):
    SearchView<Project>(ctx,
		     "Project",
		     gtk_list_store_new(5,
					G_TYPE_POINTER,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING),
		     [&ctx](auto &rec) { edit(ctx, rec); }),
    id_fld(new_id_field()),
    text_fld(gtk_entry_new()),
    peer_fld(ctx)
  {
    GtkWidget *lbl;

    auto text_box(gtk_grid_new());
    gtk_grid_set_row_spacing(GTK_GRID(text_box), 5);
    gtk_grid_set_column_spacing(GTK_GRID(text_box), 5);
    gtk_container_add(GTK_CONTAINER(fields), text_box);

    lbl = gtk_label_new("Id");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(text_box), lbl, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(text_box), id_fld, 0, 1, 1, 1);
    
    lbl = gtk_label_new("Text");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(text_box), lbl, 1, 0, 1, 1);
    gtk_widget_set_hexpand(text_fld, true);
    gtk_grid_attach(GTK_GRID(text_box), text_fld, 1, 1, 1, 1);

    lbl = gtk_label_new("Peer");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), peer_fld.ptr());

    add_col(GTK_TREE_VIEW(list), "Id", COL_ID);
    add_col(GTK_TREE_VIEW(list), "Created", COL_CREATED);
    add_col(GTK_TREE_VIEW(list), "Owner", COL_OWNER);
    add_col(GTK_TREE_VIEW(list), "Name", COL_NAME, true);

    focused = id_fld;
  }

  void ProjectSearch::find() {
    size_t cnt(0);
    
    str id_sel(trim(gtk_entry_get_text(GTK_ENTRY(id_fld))));
    str text_sel(trim(gtk_entry_get_text(GTK_ENTRY(text_fld))));
    auto &peer_sel(peer_fld.selected);
    
    for (auto key = ctx.db.projects_sort.recs.begin();
	 key != ctx.db.projects_sort.recs.end();
	 key++) {
      auto &rec(db::get(ctx.db.projects, *key));
      Project project(ctx, rec);

      if (!id_sel.empty() && find_ci(id_str(project), id_sel) == str::npos) {
	continue;
      }

      if (!text_sel.empty() &&
	  find_ci(project.name, text_sel) == str::npos &&
	  find_ci(project.info, text_sel) == str::npos) {
	continue;
      }

      if (peer_sel &&
	  project.owner_id != peer_sel->id &&
	  project.peer_ids.find(peer_sel->id) == project.peer_ids.end()) {
	continue;
      }
      
      Peer own(get_peer_id(ctx, project.owner_id));

      GtkTreeIter iter;
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter,
			 COL_PTR, &rec,
			 COL_ID, id_str(project).c_str(),
			 COL_CREATED,
			 fmt(project.created_at, "%a %b %d, %H:%M").c_str(),
			 COL_OWNER, own.name.c_str(),
			 COL_NAME, project.name.c_str(),
			 -1);
      cnt++;
    }

    gtk_widget_grab_focus(cnt ? list : id_fld);
  }
}}