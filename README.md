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
  
  
