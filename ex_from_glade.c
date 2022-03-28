// This example demonstrates the use of a glade UI file.
//
// Note: This file must be compiled with the linker flags emitted by
// $(pkg-config gmodule-2.0) so that the dynamic loader can find the
// signal handler at runtime. See the Makefile for implementation.

#include <gtk/gtk.h>

G_MODULE_EXPORT
void treeviewEvent(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
  if (event->type == GDK_BUTTON_PRESS && event->button == 3)
    g_print("Right click...\n");

  if (event->type == GDK_BUTTON_PRESS && event->button == 1)
    g_print("Selected...\n");



}

G_MODULE_EXPORT 
void btnClicked(GtkWidget *widget, gpointer user_data)
{
  g_print("Button clicked...\n");
}

G_MODULE_EXPORT 
void btnSaveClicked(GtkWidget *widget, gpointer user_data)
{
  g_print("btnSave clicked...\n");
}

G_MODULE_EXPORT 
void btnCancelClicked(GtkWidget *widget, gpointer user_data)
{
  g_print("btnCancel clicked...\n");
}


GdkPixbuf 
*create_pixbuf(const gchar *filename)
{
  GdkPixbuf *pixbuf;
  GError *error = NULL;
  pixbuf = gdk_pixbuf_new_from_file(filename, &error);
  
  if (!pixbuf){
    fprintf(stderr, "%s\n", error->message);
    g_error_free(error);
  }
  return pixbuf;
}

int main(int argc, char** argv)
{
  
  //GdkPixbuf *icon;
  //icon = create_pixbuf("Digital-Signage.png");
  
	gtk_init(&argc, &argv);

    // Create a builder object that will load the file.
    GtkBuilder* builder = gtk_builder_new();

    // Load the XML from a file.
    gtk_builder_add_from_file(builder, "ex_tree.glade", NULL);
    
    GSList *lst, *objList = gtk_builder_get_objects(builder);

    g_print("%p\n", objList);
    for (lst = objList; lst != NULL; lst = lst->next){
      g_print("%p\n", (char*)(lst->data));
    }
    

    //GtkWidget *butt1 = GTK_WIDGET(gtk_builder_get_object(builder, "hbuttonbox1"));
    ///GtkWidget *butt2 = GTK_WIDGET(gtk_builder_get_object(builder, "ok_button"));
    
    ///g_signal_connect(butt2, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);
    
    //g_signal_connect(butt1, "clicked", G_CALLBACK(btnClicked), NULL);
    // Quit the app when the window is closed.
    //g_signal_connect(GTK_WINDOW(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Get the object called 'main_window' from the file and show it.
    GObject* window = gtk_builder_get_object(builder, "window");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WINDOW_TOPLEVEL);
    //gtk_window_set_icon(GTK_WINDOW(window), icon);
    //gtk_window_fullscreen(GTK_WINDOW(window));


    gtk_widget_show_all(GTK_WIDGET(window));

    // Main loop.
    gtk_main();

    return 0;
}
