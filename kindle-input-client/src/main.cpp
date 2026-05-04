#include <gtk-2.0/gtk/gtk.h>
#include <iostream>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <poll.h>
#include <termios.h>
#include "main.ui.xml.h"
#include "ttyStream/manage.h"
#include "ttyStream/setupUsb.h"


void on_button_quit()
{
  g_print("Quit button clicked, restoring USB and exiting...\n");
  stop_and_restore();
  usbmode_mtp();
  gtk_main_quit();
}

void on_start_button_clicked()
{
    //If thread already exist
    if (stream_thread.joinable()) {
        keep_streaming = false;
        stream_thread.join();
    }

    usbmode_serial();

    // Start the stream
    keep_streaming = true;
    stream_thread = std::thread(stream_input_to_serial);
}

void on_end_button_clicked()
{
    stop_and_restore();
    usbmode_mtp();
}

// While quitting
void on_window_destroy()
{
    stop_and_restore();
    usbmode_mtp();
    gtk_main_quit();
}

GtkBuilder *gtk_builder_new_from_embedded_file(const char *data, size_t size)
{
  GError *err = NULL;
  GtkBuilder *builder = gtk_builder_new();
  gtk_builder_add_from_string(builder, data, size, &err);
  if (err)
  {
    g_printerr("gtk_builder_add_from_string(): %s\n", err->message);
    g_clear_error(&err);
    return NULL;
  }
  return builder;
}

int main(int argc, char *argv[])
{
  gtk_init(&argc, &argv);

  GtkBuilder *builder = gtk_builder_new_from_embedded_file(reinterpret_cast<const char *>(MAIN_UI_XML), MAIN_UI_XML_LEN);
  if (!builder)
  {
    g_printerr("gtk_builder_new_from_embedded_file() failed\n");
    return 1;
  }
  gtk_builder_connect_signals(builder, NULL);

  GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  GtkWidget *button_quit = GTK_WIDGET(gtk_builder_get_object(builder, "title_button_left"));
  GtkWidget *start_button = GTK_WIDGET(gtk_builder_get_object(builder, "start_button"));
  GtkWidget *end_button = GTK_WIDGET(gtk_builder_get_object(builder, "end_button"));

  // Restore MTP when closing just in case
  g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

  g_signal_connect(button_quit, "button-press-event", G_CALLBACK(on_button_quit), nullptr);
  g_signal_connect(start_button, "clicked", G_CALLBACK(on_start_button_clicked), nullptr);
  g_signal_connect(end_button, "clicked", G_CALLBACK(on_end_button_clicked), nullptr);

  GdkColor color;
  if (gdk_color_parse("#ffffff", &color))
  {
    gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &color);
  }

  gtk_widget_show(window);

  gtk_main();

  return 0;
}
