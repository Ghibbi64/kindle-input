#include <cairo.h>
#include <cmath>
#include "drawArea.h"

static GtkWidget *global_drawing_area = nullptr;

struct DimensionsUpdate {
    double pct_x;
    double pct_y;
};

static gboolean delayed_eink_refresh(gpointer data) {
    system("eips -f");
    return FALSE; // Run only once
}

static gboolean trigger_ui_redraw(gpointer data) {
    DimensionsUpdate* update = static_cast<DimensionsUpdate*>(data);

    bool dimensions_changed = (active_area_pct_x != update->pct_x) ||
                              (active_area_pct_y != update->pct_y);

    active_area_pct_x = update->pct_x;
    active_area_pct_y = update->pct_y;

    if (global_drawing_area) {
        gtk_widget_queue_draw(global_drawing_area);
    }

    if (dimensions_changed) {
        g_timeout_add(200, delayed_eink_refresh, NULL);
    }

    delete update;
    return FALSE;
}

void update_drawing_area_dimensions(double pct_x, double pct_y) {
    DimensionsUpdate* update = new DimensionsUpdate();
    update->pct_x = pct_x;
    update->pct_y = pct_y;

    g_idle_add(trigger_ui_redraw, update);
}

void init_drawing_area(GtkBuilder *builder) {
    global_drawing_area = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    if (global_drawing_area) {
        gtk_widget_set_app_paintable(global_drawing_area, TRUE);

        g_signal_connect(global_drawing_area, "expose-event", G_CALLBACK(on_drawing_area_expose), NULL);
    } else {
        g_printerr("Failed to find 'window' in UI XML!\n");
    }
}

gboolean on_drawing_area_expose(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
    cairo_t *cr = gdk_cairo_create(widget->window);

    int physical_width = widget->allocation.width;
    int physical_height = widget->allocation.height;

    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_paint(cr);

    double visual_width = physical_height;
    double visual_height = physical_width;

    double rect_w = visual_width * active_area_pct_x;
    double rect_h = visual_height * active_area_pct_y;

    double rect_x = -rect_w / 2.0;
    double rect_y = -rect_h / 2.0;

    cairo_translate(cr, physical_width / 2.0, physical_height / 2.0);
    cairo_rotate(cr, -M_PI / 2.0);

    double radius = 15.0;
    if (radius > rect_w / 2.0) radius = rect_w / 2.0;
    if (radius > rect_h / 2.0) radius = rect_h / 2.0;
    cairo_new_sub_path(cr);
    cairo_arc(cr, rect_x + rect_w - radius, rect_y + radius, radius, -M_PI / 2.0, 0);
    cairo_arc(cr, rect_x + rect_w - radius, rect_y + rect_h - radius, radius, 0, M_PI / 2.0);
    cairo_arc(cr, rect_x + radius, rect_y + rect_h - radius, radius, M_PI / 2.0, M_PI);
    cairo_arc(cr, rect_x + radius, rect_y + radius, radius, M_PI, 3.0 * M_PI / 2.0);
    cairo_close_path(cr);
    cairo_set_source_rgb(cr, 0.7, 0.7, 0.7);
    cairo_fill(cr);
    cairo_destroy(cr);

    return FALSE;
}
