// This example demonstrates the use of a glade UI file.
//
// Note: This file must be compiled with the linker flags emitted by
// $(pkg-config gmodule-2.0) so that the dynamic loader can find the
// signal handler at runtime. See the Makefile for implementation.

#include <gtk/gtk.h>

typedef struct
{
  const gchar    *time;
  const gchar    *dest;
  const gchar    *busno;
  const gchar    *standard;
  const gchar    *platform;
  const gchar    *note;
  const gboolean out;
  const gchar    *objname;
}
Table;

enum
{
  COL_TIME,
  COL_DEST,
  COL_BUSNO,
  COL_STANDARD,
  COL_PLATFORM,
  COL_NOTE,
  NUM_COLUMNS
};

static Table data[] =
{
  {"17:00", "เชียงใหม่", "18-102", "ม.1ข", "1", "", FALSE},
  {"17:30", "เชียงของ", "962-1", "ม.4ข", "3", "", FALSE},
  {"18:00", "แม่สาย", "957-3", "ม.1ก", "ประตู 3", "", FALSE},
  {"18:05", "เชียงราย", "909-12", "ม.4ข", "4", "", FALSE},
  {"18:30", "แม่ฮ่องสอน", "961-1", "ม.1พ", "5", "", FALSE},
  {"19:00", "เชียงใหม่", "18-1", "ม.1ข", "1", "", FALSE},
  {"19:15", "เชียงใหม่", "18-1", "ม.1ก", "ประตู 3", "รถมาจากหมอชิต, รถติดหน้าหมอชิต", FALSE},
  {"19:30", "เชียงราย", "909-1", "ม.1ก", "ประตู 3", "", FALSE},
  {"19:45", "น่าน", "910-2", "ม.1ข", "4", "", FALSE},
};

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
  
  GdkPixbuf *icon;
  icon = create_pixbuf("Digital-Signage.png");
  
	gtk_init(&argc, &argv);

  GtkBuilder* builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "ex_from_glade.xml", NULL);
  gtk_builder_connect_signals(builder, NULL);
  
    GSList *lst, *objList = gtk_builder_get_objects(builder);

    g_print("%p\n", objList);
    for (lst = objList; lst != NULL; lst = lst->next){
      g_print("%p\n", (char*)(lst->data));
    }
    
  GObject* window = gtk_builder_get_object(builder, "window");
  gtk_window_set_position(GTK_WINDOW(window), GTK_WINDOW_TOPLEVEL);
  gtk_window_set_icon(GTK_WINDOW(window), icon);
 
  GtkListStore *store = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));
  GtkTreeIter iter;

  int i;
  for (i = 0; i < G_N_ELEMENTS (data); i++){
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(
      store, &iter, COL_TIME, data[i].time, COL_DEST, data[i].dest, COL_BUSNO, data[i].busno, COL_STANDARD, data[i].standard, 
      COL_PLATFORM, data[i].platform, COL_NOTE, data[i].note, -1);
  }   

  gtk_widget_show_all(GTK_WIDGET(window));

  gtk_main();

  return 0;
}
