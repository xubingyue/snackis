#include <gtk/gtk.h>

static void load_style() {
  GdkDisplay *display(gdk_display_get_default());
  GdkScreen *screen(gdk_display_get_default_screen(display));
  GtkCssProvider *provider = gtk_css_provider_new();
  gtk_style_context_add_provider_for_screen(screen,
					    GTK_STYLE_PROVIDER(provider),
					    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  GError *error(nullptr);
  gtk_css_provider_load_from_file(provider, g_file_new_for_path("gui.css"), &error);
}

static void activate(GtkApplication *app, gpointer user_data) {
  load_style();
  
  GtkWidget *wnd(gtk_application_window_new(app));
  gtk_window_set_title(GTK_WINDOW(wnd), "Snackis");
  gtk_window_maximize(GTK_WINDOW(wnd));

  GtkWidget *panels(gtk_paned_new(GTK_ORIENTATION_HORIZONTAL));
  gtk_container_add(GTK_CONTAINER(wnd), panels);

  GtkWidget *left(gtk_box_new(GTK_ORIENTATION_VERTICAL, 10));

  GtkWidget *console_view(gtk_text_view_new());
  gtk_text_view_set_editable(GTK_TEXT_VIEW(console_view), false);
  GtkWidget* console = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_overlay_scrolling(GTK_SCROLLED_WINDOW(console), false);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(console),
				 GTK_POLICY_NEVER,
				 GTK_POLICY_ALWAYS);
  
  gtk_container_add(GTK_CONTAINER(console), console_view);
  gtk_box_pack_start(GTK_BOX(left), console, true, true, 0);
  
  GtkWidget *reader(gtk_entry_new());
  gtk_style_context_add_class(gtk_widget_get_style_context(reader),"reader");
  gtk_box_pack_start(GTK_BOX(left), reader, false, false, 0);

  gtk_paned_add1(GTK_PANED(panels), left);
  gtk_widget_show_all(wnd);
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int status;
  app = gtk_application_new("foo.bar.snackis", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}
