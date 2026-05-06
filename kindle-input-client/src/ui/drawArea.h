#include <gtk-2.0/gtk/gtk.h>

#ifndef DRAW_AREA_H
#define DRAW_AREA_H

void init_drawing_area(GtkBuilder *builder);
void update_drawing_area_dimensions(double pct_x, double pct_y);
gboolean on_drawing_area_expose(GtkWidget *widget, GdkEventExpose *event, gpointer data);

inline double active_area_pct_x = 0.50;
inline double active_area_pct_y = 0.50;

#endif
