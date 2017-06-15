#include "snackis/ctx.hpp"
#include "snackis/snackis.hpp"
#include "snackis/gui/console.hpp"
#include "snackis/gui/gui.hpp"
#include "snackis/gui/reader.hpp"

using namespace snackis;

gboolean on_key(gpointer ptr, GdkEventKey *ev, gpointer data) {
  if (ev->keyval == GDK_KEY_Escape && gui::reader) {
    gui::reader->focus();
    return true;
  }

  return false;
}

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
  
  gui::window = gtk_application_window_new(app);
  g_signal_connect(G_OBJECT(gui::window),
		   "key_press_event",
		   G_CALLBACK(on_key),
		   nullptr);
  gui::add_style(gui::window, "window");
  gtk_window_set_title(GTK_WINDOW(gui::window),
		       fmt("Snackis v%0", version_str()).c_str());
  gtk_window_maximize(GTK_WINDOW(gui::window));

  gui::main_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(gui::window), gui::main_panel);
  
  gui::panels = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous(GTK_BOX(gui::panels), true);
  gtk_box_pack_start(GTK_BOX(gui::main_panel), gui::panels, true, true, 0);

  Ctx *ctx = new Ctx("db/");
  gui::left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_container_add(GTK_CONTAINER(gui::panels), gui::left_panel);
  gui::console.emplace();
  ctx->log = [](const str &msg) { gui::log(*gui::console, msg); };
  gtk_box_pack_start(GTK_BOX(gui::left_panel), gui::console->ptr(), true, true, 0);

  gui::Login *login = new gui::Login(*ctx);
  login->push_view();
  gtk_widget_show_all(gui::window);
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
