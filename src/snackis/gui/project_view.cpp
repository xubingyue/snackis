#include <cassert>
#include "snackis/ctx.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/feed_view.hpp"
#include "snackis/gui/project_view.hpp"

namespace snackis {
namespace gui {
  ProjectView::ProjectView(const Project &project):
    RecView<Project>("Project", project),
    name_fld(gtk_entry_new()),
    active_fld(gtk_check_button_new_with_label("Active")),    
    info_fld(new_text_view()) {
    GtkWidget *lbl;

    lbl = gtk_label_new("Name");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    GtkWidget *name_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(fields), name_box);
    gtk_widget_set_hexpand(name_fld, true);    
    gtk_container_add(GTK_CONTAINER(name_box), name_fld);
    gtk_entry_set_text(GTK_ENTRY(name_fld), project.name.c_str());
    gtk_container_add(GTK_CONTAINER(name_box), active_fld);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active_fld), project.active);
    
    lbl = gtk_label_new("Info");
    gtk_widget_set_halign(lbl, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(fields), lbl);
    gtk_container_add(GTK_CONTAINER(fields), gtk_widget_get_parent(info_fld));
    set_str(GTK_TEXT_VIEW(info_fld), project.info);
    
    focused = name_fld;
  }

  bool ProjectView::allow_save() const {
    return rec.owner_id == whoami(ctx).id;
  }

  bool ProjectView::save() {
    rec.name = gtk_entry_get_text(GTK_ENTRY(name_fld));
    rec.info = get_str(GTK_TEXT_VIEW(info_fld));
    db::upsert(ctx.db.projects, rec);
    return true;
  }
}}
