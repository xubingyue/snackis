#include "snackis/ctx.hpp"
#include "snackis/core/defer.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/post_search.hpp"
#include "snackis/gui/post_view.hpp"
#include "snackis/gui/script_view.hpp"

namespace snackis {
namespace gui {
  enum BCodeCols { BCODE_PTR=0, BCODE_NAME, BCODE_INFO };

  static void on_page(GtkNotebook *w, GtkWidget *p, guint pn, ScriptView *v) {
    switch (pn) {
    case 2:
      load(v->peer_lst);
      break;
    case 3:
      clear(v->post_lst);
      auto fd(find_feed_id(v->ctx, v->rec.id));
      if (fd) { load(v->post_lst, *fd, now()); }
      break;
    }
  }
  
  static void on_new_script(gpointer *_, ScriptView *v) {
    Script prj(v->ctx);
    prj.peer_ids = v->rec.peer_ids;
    prj.name = trim(fmt("*COPY* %0", v->rec.name));
    prj.tags = v->rec.tags;
    prj.code = trim(fmt("*COPY*\n%0", v->rec.code));

    auto pv(new ScriptView(prj));
    pv->focused = pv->name_fld;
    push_view(pv);
  }
  
  static void on_find_posts(gpointer *_, ScriptView *v) {
    PostSearch *ps = new PostSearch(v->ctx);
    select<Feed>(ps->feed_fld, get_feed(v->rec));
    push_view(ps);
  }

  static void on_post(gpointer *_, ScriptView *v) {
    db::Trans trans(v->ctx);
    TRY(try_save);
    v->save();
    
    if (try_save.errors.empty()) {
      db::commit(trans, fmt("Saved Script %0", id_str(v->rec)));
      Post post(v->ctx);
      post.feed_id = get_feed(v->rec).id;
      push_view(new PostView(post));
    }
  }

  static void on_generate(gpointer *_, ScriptView *v) {
    TRY(try_generate);
    snabel::compile(v->exec.main, get_str(GTK_TEXT_VIEW(v->code_fld)));    
    gtk_list_store_clear(v->bcode_store);

    for (auto &op: v->exec.main.ops) {
      GtkTreeIter iter;
      gtk_list_store_append(v->bcode_store, &iter);
      
      gtk_list_store_set(v->bcode_store, &iter,
			 BCODE_PTR, &op,
			 BCODE_NAME, op.name.c_str(),
			 BCODE_INFO, info(op, curr_scope(v->exec.main)).c_str(),
			 -1);
    }

    log(v->ctx, "Bytecode generated");
    
    if (try_generate.errors.empty()) {
      gtk_widget_grab_focus(v->bcode_lst);
      gtk_widget_set_sensitive(v->run_btn, true);
    } else {
      gtk_widget_grab_focus(v->code_fld);
      gtk_widget_set_sensitive(v->run_btn, false);
    }
  }

  static void on_run(gpointer *_, ScriptView *v) {
    TRY(try_run);
    auto &cor(v->exec.main);
    rewind(cor);
    begin_scope(cor);
    snabel::run(cor);
    end_scope(cor);
    log(v->ctx, "Result: %0", pop(v->exec.main));
  }
  
  static GtkWidget *init_code(ScriptView &v) {
    const UId me(whoamid(v.ctx));
    GtkWidget *frm(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5));
    gtk_widget_set_margin_top(frm, 5);
    gtk_box_set_homogeneous(GTK_BOX(frm), true);

    GtkWidget *left(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    gtk_container_add(GTK_CONTAINER(frm), left);
    
    gtk_container_add(GTK_CONTAINER(left), new_label("Code"));
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(v.code_fld), true);

    if (v.rec.owner_id != me) { read_only(GTK_TEXT_VIEW(v.code_fld)); }
    gtk_container_add(GTK_CONTAINER(left), gtk_widget_get_parent(v.code_fld));
    set_str(GTK_TEXT_VIEW(v.code_fld), v.rec.code);

    gtk_widget_set_halign(v.generate_btn, GTK_ALIGN_START);
    g_signal_connect(v.generate_btn, "clicked", G_CALLBACK(on_generate), &v);
    gtk_container_add(GTK_CONTAINER(left), v.generate_btn);

    GtkWidget *right(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    gtk_container_add(GTK_CONTAINER(frm), right);
    
    gtk_container_add(GTK_CONTAINER(right), new_label("Bytecode"));
    add_col(GTK_TREE_VIEW(v.bcode_lst), "Name", BCODE_NAME);
    add_col(GTK_TREE_VIEW(v.bcode_lst), "Info", BCODE_INFO, true);
    gtk_container_add(GTK_CONTAINER(right), gtk_widget_get_parent(v.bcode_lst));

    gtk_widget_set_halign(v.run_btn, GTK_ALIGN_START);
    gtk_widget_set_sensitive(v.run_btn, false);
    g_signal_connect(v.run_btn, "clicked", G_CALLBACK(on_run), &v);
    gtk_container_add(GTK_CONTAINER(right), v.run_btn);
    
    return frm;
  }

  static GtkWidget *init_general(ScriptView &v) {
    const UId me(whoamid(v.ctx));
    GtkWidget *frm(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    gtk_widget_set_margin_top(frm, 5);

    gtk_container_add(GTK_CONTAINER(frm), new_label("Name"));
    gtk_widget_set_hexpand(v.name_fld, true);
    if (v.rec.owner_id != me) { read_only(GTK_ENTRY(v.name_fld)); }
    gtk_container_add(GTK_CONTAINER(frm), v.name_fld);
    gtk_entry_set_text(GTK_ENTRY(v.name_fld), v.rec.name.c_str());

    gtk_container_add(GTK_CONTAINER(frm), new_label("Tags"));
    gtk_widget_set_hexpand(v.tags_fld, true);
    gtk_container_add(GTK_CONTAINER(frm), v.tags_fld);    
    gtk_entry_set_text(GTK_ENTRY(v.tags_fld),
		       join(v.rec.tags.begin(), v.rec.tags.end(), ' ').c_str());
    
    return frm;
  }
  
  ScriptView::ScriptView(const Script &rec):
    SharedView<Script>("Script", rec),
    bcode_store(gtk_list_store_new(3,
				   G_TYPE_POINTER,
				   G_TYPE_STRING, G_TYPE_STRING)),
    new_script_btn(gtk_button_new_with_mnemonic("_New Script")),
    find_posts_btn(gtk_button_new_with_mnemonic("Find Posts")),
    post_btn(gtk_button_new_with_mnemonic("New _Post")),
    name_fld(gtk_entry_new()),
    tags_fld(gtk_entry_new()),
    code_fld(new_text_view()),
    bcode_lst(new_tree_view(GTK_TREE_MODEL(bcode_store))),
    generate_btn(gtk_button_new_with_mnemonic("_Generate Bytecode")),
    run_btn(gtk_button_new_with_mnemonic("_Run Bytecode")),
    peer_lst(ctx, "Peer", this->rec.peer_ids),
    post_lst(ctx)
  {
    const UId me(whoamid(ctx));

    g_signal_connect(new_script_btn, "clicked", G_CALLBACK(on_new_script), this);
    gtk_container_add(GTK_CONTAINER(menu), new_script_btn);
    gtk_widget_set_sensitive(find_posts_btn,
			     find_feed_id(ctx, rec.id) ? true : false);
    g_signal_connect(find_posts_btn, "clicked", G_CALLBACK(on_find_posts), this);
    gtk_container_add(GTK_CONTAINER(menu), find_posts_btn);
    g_signal_connect(post_btn, "clicked", G_CALLBACK(on_post), this);
    gtk_container_add(GTK_CONTAINER(menu), post_btn);
    
    GtkWidget *tabs(gtk_notebook_new());
    gtk_widget_set_vexpand(tabs, true);
    g_signal_connect(tabs, "switch-page", G_CALLBACK(on_page), this);
    gtk_container_add(GTK_CONTAINER(fields), tabs);

    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     init_general(*this),
			     gtk_label_new_with_mnemonic("_1 General"));

    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     init_code(*this),
			     gtk_label_new_with_mnemonic("_2 Code"));

    if (rec.owner_id != me) { read_only(peer_lst); }

    gtk_notebook_append_page(GTK_NOTEBOOK(tabs),
			     peer_lst.ptr(),
			     gtk_label_new_with_mnemonic("_3 Peers"));

    GtkWidget *l(gtk_label_new_with_mnemonic("_4 Post History"));
    if (!find_feed_id(ctx, rec.id)) { gtk_widget_set_sensitive(l, false); }
    gtk_notebook_append_page(GTK_NOTEBOOK(tabs), post_lst.ptr(), l);

    focused = name_fld;
  }

  bool ScriptView::save() {
    rec.name = gtk_entry_get_text(GTK_ENTRY(name_fld));
    rec.tags = word_set(get_str(GTK_ENTRY(tags_fld)));
    rec.code = get_str(GTK_TEXT_VIEW(code_fld));
    db::upsert(ctx.db.scripts, rec);
    return true;
  }
}}
