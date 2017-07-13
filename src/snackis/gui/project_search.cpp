#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/project_search.hpp"
#include "snackis/gui/project_view.hpp"

namespace snackis {
namespace gui {
  enum ProjectCol {COL_PTR=0, COL_ID, COL_CREATED, COL_OWNER, COL_NAME, COL_TAGS,
		   COL_INFO};

  static void edit(Ctx &ctx, const db::Rec<Project> &rec) {
    push_view(new ProjectView(Project(ctx, rec)));
  }
  
  ProjectSearch::ProjectSearch(Ctx &ctx):
    SearchView<Project>(ctx,
		     "Project",
		     gtk_list_store_new(7,
					G_TYPE_POINTER,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING),
		     [&ctx](auto &rec) { edit(ctx, rec); }),
    id_fld(new_id_field()),
    tags_fld(gtk_entry_new()),
    text_fld(gtk_entry_new()),
    peer_fld(ctx)
  {
    auto frm(gtk_grid_new());
    gtk_grid_set_row_spacing(GTK_GRID(frm), 5);
    gtk_grid_set_column_spacing(GTK_GRID(frm), 5);
    gtk_container_add(GTK_CONTAINER(fields), frm);
    int row = 0;
    
    gtk_grid_attach(GTK_GRID(frm), new_label("Id"), 0, row, 1, 1);
    gtk_widget_set_halign(id_fld, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(frm), id_fld, 0, row+1, 1, 1);

    row += 2;
    gtk_grid_attach(GTK_GRID(frm), new_label("Tags"), 0, row, 1, 1);
    gtk_widget_set_hexpand(tags_fld, true);
    gtk_grid_attach(GTK_GRID(frm), tags_fld, 0, row+1, 1, 1);

    gtk_grid_attach(GTK_GRID(frm), new_label("Text"), 1, row, 1, 1);
    gtk_widget_set_hexpand(text_fld, true);
    gtk_grid_attach(GTK_GRID(frm), text_fld, 1, row+1, 1, 1);

    row += 2;
    gtk_grid_attach(GTK_GRID(frm), new_label("Owner"), 0, row, 2, 1);
    gtk_grid_attach(GTK_GRID(frm), peer_fld.ptr(), 0, row+1, 2, 1);

    add_col(GTK_TREE_VIEW(list), "Id", COL_ID);
    add_col(GTK_TREE_VIEW(list), "Created", COL_CREATED);
    add_col(GTK_TREE_VIEW(list), "Owner", COL_OWNER);
    add_col(GTK_TREE_VIEW(list), "Name", COL_NAME, true);
    add_col(GTK_TREE_VIEW(list), "Tags", COL_TAGS);    
    add_col(GTK_TREE_VIEW(list), "Info", COL_INFO, true);

    focused = id_fld;
  }

  void ProjectSearch::find() {
    size_t cnt(0);
    
    str id_sel(trim(gtk_entry_get_text(GTK_ENTRY(id_fld))));
    str tags_str(trim(gtk_entry_get_text(GTK_ENTRY(tags_fld))));
    std::set<str> tags_sel(word_set(tags_str));
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

      if (!tags_sel.empty()) {
	std::vector<str> diff;
	std::set_difference(tags_sel.begin(), tags_sel.end(),
			    project.tags.begin(), project.tags.end(),
			    std::back_inserter(diff));
	if (!diff.empty()) { continue; }
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
			 COL_TAGS,
			 join(project.tags.begin(), project.tags.end(), '\n').c_str(),
			 COL_INFO, project.info.c_str(),			 
			 -1);
      cnt++;
    }

    gtk_widget_grab_focus(cnt ? list : id_fld);
  }
}}
