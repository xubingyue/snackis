#include <gtk/gtk.h>

static void activate(GtkApplication *app, gpointer user_data) {
  GtkWindow *wnd(GTK_WINDOW(gtk_application_window_new(app)));
  gtk_window_set_title(wnd, "Snackis");
  gtk_window_maximize(wnd);
  gtk_window_present(wnd);
}

int main(int argc, char **argv) {
  GtkApplication *app;
  int status;
  app = gtk_application_new("snackis", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return (status);
}
