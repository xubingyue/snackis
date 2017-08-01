#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/script_search.hpp"
#include "snackis/gui/script_view.hpp"

namespace snackis {
namespace gui {
  enum ScriptCol {COL_PTR=0, COL_ID, COL_CREATED, COL_OWNER, COL_TAGS, COL_NAME};

  static void edit(Ctx &ctx, const db::Rec<Script> &rec) {
    push_view(new ScriptView(Script(ctx, rec)));
  }
  
  ScriptSearch::ScriptSearch(Ctx &ctx):
    SearchView<Script>(ctx,
		     "Script",
		     gtk_list_store_new(6,
					G_TYPE_POINTER,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING,
					G_TYPE_STRING),
		     [&ctx](auto &rec) { edit(ctx, rec); }),
    id_fld(new_id_field()),
    tags_fld(gtk_entry_new()),
    code_fld(gtk_entry_new()),
    peer_fld(ctx)
  {
    auto frm(new_grid());
    gtk_container_add(GTK_CONTAINER(fields), frm);
    int row(0);
    
    gtk_grid_attach(GTK_GRID(frm), new_label("Id"), 0, row, 1, 1);
    gtk_widget_set_halign(id_fld, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(frm), id_fld, 0, row+1, 1, 1);
    
    row += 2;
    gtk_grid_attach(GTK_GRID(frm), new_label("Tags"), 0, row, 1, 1);
    gtk_widget_set_hexpand(tags_fld, true);
    gtk_grid_attach(GTK_GRID(frm), tags_fld, 0, row+1, 1, 1);

    gtk_grid_attach(GTK_GRID(frm), new_label("Code"), 1, row, 1, 1);
    gtk_widget_set_hexpand(code_fld, true);
    gtk_grid_attach(GTK_GRID(frm), code_fld, 1, row+1, 1, 1);

    row += 2;
    gtk_grid_attach(GTK_GRID(frm), new_label("Owner"), 0, row, 2, 1);
    gtk_grid_attach(GTK_GRID(frm), peer_fld.ptr(), 0, row+1, 2, 1);

    add_col(GTK_TREE_VIEW(list), "Id", COL_ID);
    add_col(GTK_TREE_VIEW(list), "Created", COL_CREATED);
    add_col(GTK_TREE_VIEW(list), "Owner", COL_OWNER);
    add_col(GTK_TREE_VIEW(list), "Tags", COL_TAGS);    
    add_col(GTK_TREE_VIEW(list), "Name", COL_NAME, true);

    focused = id_fld;
  }

  void ScriptSearch::find() {
    size_t cnt(0);
    
    str id_sel(trim(gtk_entry_get_text(GTK_ENTRY(id_fld))));
    str tags_str(trim(gtk_entry_get_text(GTK_ENTRY(tags_fld))));
    std::set<str> tags_sel(word_set(tags_str));
    str code_sel(trim(gtk_entry_get_text(GTK_ENTRY(code_fld)))); 
    auto &peer_sel(peer_fld.selected);
    
    for (const auto &key: ctx.db.scripts_sort.recs) {
      auto &rec(db::get(ctx.db.scripts, key.second));
      Script script(ctx, rec);

      if (!id_sel.empty() && find_ci(id_str(script), id_sel) == str::npos) {
	continue;
      }

      if (!tags_sel.empty()) {
	std::vector<str> diff;
	std::set_difference(tags_sel.begin(), tags_sel.end(),
			    script.tags.begin(), script.tags.end(),
			    std::back_inserter(diff));
	if (!diff.empty()) { continue; }
      }
      
      if (!code_sel.empty() && find_ci(script.code, code_sel) == str::npos) {
	continue;
      }

      if (peer_sel &&
	  script.owner_id != peer_sel->id &&
	  script.peer_ids.find(peer_sel->id) == script.peer_ids.end()) {
	continue;
      }
      
      Peer own(get_peer_id(ctx, script.owner_id));

      GtkTreeIter iter;
      gtk_list_store_append(store, &iter);
      gtk_list_store_set(store, &iter,
			 COL_PTR, &rec,
			 COL_ID, id_str(script).c_str(),
			 COL_CREATED,
			 fmt(script.created_at, "%a %b %d, %H:%M").c_str(),
			 COL_OWNER, own.name.c_str(),
			 COL_TAGS,
			 join(script.tags.begin(), script.tags.end(), '\n').c_str(),
			 COL_NAME, script.name.c_str(), 
			 -1);
      cnt++;
    }

    gtk_widget_grab_focus(cnt ? list : id_fld);
  }
}}
