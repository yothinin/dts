// This example demonstrates the use of a glade UI file.
//
// Note: This file must be compiled with the linker flags emitted by
// $(pkg-config gmodule-2.0) so that the dynamic loader can find the
// signal handler at runtime. See the Makefile for implementation.

#include <gtk/gtk.h>

GtkBuilder* builder;
GtkWidget *entTime, *entDest, *entBusNo, *entPlatform, *entNote;
GtkTreeView *treeview = NULL;
GtkTreeModel *model = NULL;
GtkListStore *liststore = NULL;
GtkTreeIter iter;

G_MODULE_EXPORT 
void btnClicked(GtkWidget *widget, gpointer user_data)
{
  g_print("Button clicked...\n");
}

G_MODULE_EXPORT 
void btnSaveClicked(GtkWidget *widget, gpointer user_data)
{
  g_print("btnSave clicked...\n");
  entTime = GTK_WIDGET(gtk_builder_get_object(builder, "entTime"));
  entDest = GTK_WIDGET(gtk_builder_get_object(builder, "entDest"));
  entBusNo = GTK_WIDGET(gtk_builder_get_object(builder, "entBusNo"));
  entPlatform = GTK_WIDGET(gtk_builder_get_object(builder, "entPlatform"));
  entNote = GTK_WIDGET(gtk_builder_get_object(builder, "entNote"));
  g_print("ข้อมูล:\n%s\n%s\n%s\n%s\n%s\n", 
    gtk_entry_get_text(GTK_ENTRY(entTime)), 
    gtk_entry_get_text(GTK_ENTRY(entDest)),
    gtk_entry_get_text(GTK_ENTRY(entBusNo)),
    gtk_entry_get_text(GTK_ENTRY(entPlatform)),
    gtk_entry_get_text(GTK_ENTRY(entNote))
  );
  
  // set treeview data
  //gchar* data = "17:00";
  //gchar* item = NULL;
  
  treeview = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview1"));
  model = gtk_tree_view_get_model(treeview);
  liststore = GTK_LIST_STORE (model);
  
  

  gtk_list_store_append(liststore, &iter);
  //gtk_list_store_set(liststore, &iter, 0, data, -1);
  
  /*
  gtk_list_store_set(liststore, &iter, 
    0, "18:00", 
    1, "18-102", 
    2, "ม.1ก", 
    3, "ประตู 3", 
    4, "---", 
    -1
  );
  */

  //gtk_tree_model_get(model, &iter, 0, item, -1);
  
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
  GdkPixbuf *icon;
  icon = create_pixbuf("Digital-Signage.png");
  
	gtk_init(&argc, &argv);

    // Create a builder object that will load the file.
    builder = gtk_builder_new();

    // Load the XML from a file.
    gtk_builder_add_from_file(builder, "dts_scr_time.xml", NULL);
    
    GSList *lst, *objList = gtk_builder_get_objects(builder);

    g_print("%p\n", objList);
    for (lst = objList; lst != NULL; lst = lst->next){
      g_print("%p\n", (char*)(lst->data));
    }
    
    // Get the object called 'main_window' from the file and show it.
    GObject* window = gtk_builder_get_object(builder, "window");
    gtk_window_set_icon(GTK_WINDOW(window), icon);


    //GtkWidget *butt1 = GTK_WIDGET(gtk_builder_get_object(builder, "hbuttonbox1"));
    ///GtkWidget *butt2 = GTK_WIDGET(gtk_builder_get_object(builder, "ok_button"));
    
    ///g_signal_connect(butt2, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);
    
    //g_signal_connect(butt1, "clicked", G_CALLBACK(btnClicked), NULL);
    // Quit the app when the window is closed.
    //g_signal_connect(GTK_WINDOW(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(GTK_WIDGET(window));

    // Main loop.
    gtk_main();

    return 0;
}
