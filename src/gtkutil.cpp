#include "gtkutil.h"

#include <gtk/gtk.h>
#include <gdk/gdkx.h>

void *get_gtk_native_window_handle(void *widget) {
    GtkWidget *gtk_widget = GTK_WIDGET(widget);

    gtk_widget_realize(gtk_widget);

    GdkWindow *gdk_window = gtk_widget_get_window(gtk_widget);
    Window xid = gdk_x11_window_get_xid(gdk_window);

    return reinterpret_cast<void*>(xid);
}
