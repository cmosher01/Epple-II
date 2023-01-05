/* 
 * File:   gtkutil.h
 * Author: user
 *
 * Created on January 4, 2023, 7:18 AM
 */

#ifndef GTKUTIL_H
#define GTKUTIL_H

#ifdef __WXGTK__

void *get_gtk_native_window_handle(void *widget);

#endif

#endif /* GTKUTIL_H */
