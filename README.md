# dts
Simple display bus schedule. 

Compile:

gcc -g -Wall -o dts-simple dts-simple.c dts_functions.c  `pkg-config --libs --cflags gtk+-2.0` 
-L/usr/lib/aarch64-linux-gnu/ -lmariadb -DGTK_DISABLE_DEPRECATE

Compile resource:

1. Create resource xml file.

<?xml version="1.0" encoding="UTF-8"?>
<gresources>
  <gresource prefix="/com/bustecz/dts">
    <file>ex_from_glade.xml</file>
  </gresource>
</gresources>

2. Convert

glib-compile-resources res_from_glade.xml --target=ex_from_glade-res.c --generate-source

3. Include

#include "ex_from_glade-res.c"

4. Change builder from file to resource.

  GtkBuilder* builder = gtk_builder_new();
  
  //gtk_builder_add_from_file(builder, "ex_from_glade.xml", NULL);
  
  builder = gtk_builder_new_from_resource("/com/bustecz/dts/ex_from_glade.xml");
  
  
Compile and build with xml: Don't forgot options -rdynamic -export-dynamic

Orange Pi 3 LTS: "Debian GNU/Linux 10 (buster)" 5.10.75-sunxi64

orangepi@orangepi3-lts:~/projects/dts$ pkg-config gtk+-3.0 --cflags --libs
-pthread -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 
-I/usr/lib/aarch64-linux-gnu/dbus-1.0/include -I/usr/include/gtk-3.0 -I/usr/include/gio-unix-2.0 -I/usr/include/cairo 
-I/usr/include/libdrm -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/pango-1.0 -I/usr/include/fribidi 
-I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng16 
-I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/uuid -I/usr/include/glib-2.0 
-I/usr/lib/aarch64-linux-gnu/glib-2.0/include 
-lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject 
-lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0

