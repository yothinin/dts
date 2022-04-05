// This example demonstrates the use of a glade UI file.
//
// Note: This file must be compiled with the linker flags emitted by
// $(pkg-config gmodule-2.0) so that the dynamic loader can find the
// signal handler at runtime. See the Makefile for implementation.

#include <gtk/gtk.h>
#include <mysql.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>
//#include "ex_from_glade-res.c"

GtkBuilder* builder;
MYSQL *cnx_init;
MYSQL *cnx_db;
MYSQL_RES *result_set;
MYSQL_ROW *row;

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

typedef struct
{
  const gchar *std_code;
  const gchar *std_name;
}
Standard;

typedef struct
{
  const gchar *dest_code;
  const gchar *dest_name;
}
Destination;

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

static Standard std[] = 
{
  {"11", "ม.1ก"},
  {"12", "ม.1ข"},
  {"13", "ม.1พ"},
  {"20", "ม.2"},
  {"41", "ม.4ก"},
  {"42", "ม.4ข"},
  {"43", "ม.4ค"},
  {"44", "ม.4พ"},  
};

static Destination dest[] = 
{
  {"3", "เชียงแสน"},
  {"13", "ลำพูน"},
  {"18", "เชียงใหม่"},
  {"20", "ขอนแก่น"},
  {"31", "สุรินทร์"},
  {"47", "ทุ่งช้าง"},
  {"909", "เชียงราย"},
  {"910", "น่าน"},
  {"922", "แพร่"},
  {"923", "พะเยา"},
  {"957", "แม่สาย"},
  {"962", "เชียงของ"},
};

void
db_init()
{
  g_print("Start connect to sql...\n");
  cnx_init = mysql_init(NULL);
  if (cnx_init == NULL){
    g_print("connect failed in mysql_init, \n");
    g_print("exit code: 1\n");
    exit(1);
  }
}

void
db_connect()
{
  cnx_db = mysql_real_connect(cnx_init, "localhost", "orangepi", "0rangePi", "dts", 0, NULL, 0);
  if (cnx_db == NULL){
    g_print("MySQL failure to connect to database...\n");
    g_print("Exit code: 2\n");
    g_print("Error: %u -- %s\n", mysql_errno(cnx_init), mysql_error(cnx_init));
    exit(2);
  }
  
  g_print("Database connected.\n");
}

void
db_insert(gchar *sql)
{
  if (mysql_query(cnx_init, sql) != 0){
    g_print("Failure to insert to database.");
    g_print("Exit code: 3\n");
    g_print("ERror: %u -- %s\n", mysql_errno(cnx_init), mysql_error(cnx_init));
    exit(3);
  }
  g_print("Insert data to mysql completed.\n");
}

void
db_close()
{
  mysql_close(cnx_init);
  g_print("MySQL disconnected.\n");
}

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
  GtkWidget *entHour, *entMinute;
  GtkWidget *entDest, *entRoute, *entBusNo;
  GtkWidget *entStandard;
  GtkWidget *entPlatform;
  GtkWidget *entNote;
  entHour = GTK_WIDGET(gtk_builder_get_object(builder, "entHour"));
  entMinute = GTK_WIDGET(gtk_builder_get_object(builder, "entMinute"));
  entDest = GTK_WIDGET(gtk_builder_get_object(builder, "entDest"));
  entRoute = GTK_WIDGET(gtk_builder_get_object(builder, "entRoute"));
  entBusNo = GTK_WIDGET(gtk_builder_get_object(builder, "entBusNo"));
  entStandard = GTK_WIDGET(gtk_builder_get_object(builder, "entStandard"));
  entPlatform = GTK_WIDGET(gtk_builder_get_object(builder, "entPlatform"));
  entNote = GTK_WIDGET(gtk_builder_get_object(builder, "entNote"));
  
  const gchar *depHour = gtk_entry_get_text(GTK_ENTRY(entHour));
  const gchar *depMinute = gtk_entry_get_text(GTK_ENTRY(entMinute));
  const gchar *depDest = gtk_entry_get_text(GTK_ENTRY(entDest));
  const gchar *depRoute = gtk_entry_get_text(GTK_ENTRY(entRoute));
  const gchar *depBusNo = gtk_entry_get_text(GTK_ENTRY(entBusNo));
  const gchar *depStandard = gtk_entry_get_text(GTK_ENTRY(entStandard));
  const gchar *depPlatform = gtk_entry_get_text(GTK_ENTRY(entPlatform));
  const gchar *depNote = gtk_entry_get_text(GTK_ENTRY(entNote));
  
  gchar buf_sql[256];
  g_sprintf(buf_sql, "INSERT INTO dts_depart VALUES(dep_time, dep_dest, dep_busno, dep_standard, dep_platform, dep_note, dep_datetime) VALUES ('%s:%s', '%s', '%s-%s', '%s', '%s', '%s', '%s')", depHour, depMinute, depDest, depRoute, depBusNo, depStandard, depPlatform, depNote, "2022-04-06,20:00:00");

  db_insert(buf_sql);
  
  g_free(buf_sql);
  
}

G_MODULE_EXPORT 
void btnCancelClicked(GtkWidget *widget, gpointer user_data)
{
  g_print("btnCancel clicked...\n");
}

G_MODULE_EXPORT
gboolean dest_change(GtkWidget *widget, gpointer user_data)
{

  GtkWidget *entDest;
  GtkWidget *cmbDest;
  GtkWidget *entBusNo;
  
  cmbDest = GTK_WIDGET(gtk_builder_get_object(builder, "cmbDest"));
  entDest = GTK_WIDGET(gtk_builder_get_object(builder, "entDest"));
  entBusNo = GTK_WIDGET(gtk_builder_get_object(builder, "entBusNo"));
  
  gtk_combo_box_set_id_column(GTK_COMBO_BOX(cmbDest), 1);
  gtk_entry_set_text(GTK_ENTRY(entDest), gtk_combo_box_get_active_id(GTK_COMBO_BOX(cmbDest)));
  gtk_combo_box_set_id_column(GTK_COMBO_BOX(cmbDest), 0);
  gtk_entry_grab_focus_without_selecting(GTK_ENTRY(entBusNo));

  g_print("Destination selected.\n");
  return TRUE;
}

G_MODULE_EXPORT
void std_change(GtkWidget *widget, gpointer user_data)
{
  GtkWidget *entHour = GTK_WIDGET(gtk_builder_get_object(builder, "entHour"));
  gtk_entry_grab_focus_without_selecting(GTK_ENTRY(entHour));
  g_print("Standard selected.\n");
}

void dest_changed(GtkWidget *widget, gpointer user_data)
{
  g_print("Destination changed...\n");
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

int main(int argc, char *argv[])
{

  GdkPixbuf *icon;
  GSList *lst, *objList;
  GObject* window;
  GtkListStore *store, *store_std, *store_dest;
  GtkTreeIter iter, iter2, iter3;
  
  icon = create_pixbuf("Digital-Signage.png");

  gtk_init(&argc, &argv);

  ///builder = gtk_builder_new();
  ///gtk_builder_add_from_file(builder, "ex_tree.glade", NULL);
  builder = gtk_builder_new_from_file("ex_tree.glade");
  
  //builder = gtk_builder_new_from_resource("/com/bustecz/dts/ex_from_glade.xml");
  gtk_builder_connect_signals(builder, NULL);

    objList = gtk_builder_get_objects(builder);

    g_print("%p\n", objList);
    for (lst = objList; lst != NULL; lst = lst->next){
      g_print("%p\n", (char*)(lst->data));
    }

  window = gtk_builder_get_object(builder, "window");
  gtk_window_set_position(GTK_WINDOW(window), GTK_WINDOW_TOPLEVEL);
  gtk_window_set_icon(GTK_WINDOW(window), icon);

  store = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));

  int i;
  for (i = 0; i < G_N_ELEMENTS (data); i++){
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(
      store, &iter, COL_TIME, data[i].time, COL_DEST, data[i].dest, COL_BUSNO, data[i].busno, COL_STANDARD, data[i].standard, 
      COL_PLATFORM, data[i].platform, COL_NOTE, data[i].note, -1);
  }
  
  store_std = GTK_LIST_STORE(gtk_builder_get_object(builder, "listStandard"));
  for (i = 0; i < G_N_ELEMENTS(std); ++i){
    gtk_list_store_append(store_std, &iter2);
    gtk_list_store_set(
      store_std, &iter2, 0, std[i].std_code, 1, std[i].std_name, -1);
  }
  
  store_dest = GTK_LIST_STORE(gtk_builder_get_object(builder, "listDest"));
  for (i = 0; i < G_N_ELEMENTS(dest); ++i){
    gtk_list_store_append(store_dest, &iter3);
    gtk_list_store_set(
      store_dest, &iter3, 0, dest[i].dest_code, 1, dest[i].dest_name, -1);
  }

  GtkWidget *entBusNo = GTK_WIDGET(gtk_builder_get_object(builder, "entRoute"));
  GtkWidget *cmbDest = GTK_WIDGET(gtk_builder_get_object(builder, "cmbDest"));
  
  g_object_bind_property (cmbDest, "active-id",
                          entBusNo, "text",
                          G_BINDING_BIDIRECTIONAL);

  GtkWidget *entStandard = GTK_WIDGET(gtk_builder_get_object(builder, "entStandard"));
  GtkWidget *cmbStandard = GTK_WIDGET(gtk_builder_get_object(builder, "cmbStandard"));
  g_object_bind_property (cmbStandard, "active-id",
                          entStandard, "text",
                          G_BINDING_BIDIRECTIONAL);

  db_init();
  db_connect();
  db_close();  

  gtk_widget_show_all(GTK_WIDGET(window));

  gtk_main();

  return 0;
}
