#include <gtk-2.0/gtk/gtk.h>
#include <thread>
#include "drawArea.h"
#include "../ttyStream/inputStream.h"
#include "../ttyStream/setupUsb.h"

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

void on_window_destroy()
{
    stop_and_restore();
    usbmode_mtp();
    gtk_main_quit();
}
