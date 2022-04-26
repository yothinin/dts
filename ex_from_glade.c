// This example demonstrates the use of a glade UI file.
//
// Note: This file must be compiled with the linker flags emitted by
// $(pkg-config gmodule-2.0) so that the dynamic loader can find the
// signal handler at runtime. See the Makefile for implementation.
#include <stdio.h>
#include <gtk/gtk.h>
#include <mysql.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include "dts.h"

FILE *fp;
#define LOG(X, Y) fprintf (fp, #X ": Time:%s, File:%s(%d) " #Y  "\n", __TIMESTAMP__, __FILE__, __LINE__)

//#include "ex_from_glade-res.c"

GtkBuilder* builder;
GtkWidget *window;
GtkWidget *treeview;
GtkTreeSelection *selection;

GtkWidget *entHour, *entMinute;
GtkWidget *entDest, *entRoute, *entBusNo;
GtkWidget *entStandard;
GtkWidget *entPlatform;
GtkWidget *entNote;

gint dts_mode = 0; // 0 = Insert, 1 = Update
gchar *SERVER;

MYSQL *cnx_init;
MYSQL *cnx_db;
MYSQL_RES *result_set;
MYSQL_ROW row;

typedef struct
{
  const gchar    *busno;
  const gchar    *dest;
  const gchar    *standard;
  const gchar    *time;
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
  COL_BUSNO,
  COL_DEST,
  COL_STANDARD,
  COL_TIME,
  COL_PLATFORM,
  COL_NOTE,
  NUM_COLUMNS
};

static Standard std[] =
{
  {" ", " "},
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
  {" ", " "},
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

gchar 
*config_get_string(gchar *file, gchar *group, gchar *key)
{
  GKeyFile *key_file;
  GError *error;
  //gsize length;
  gchar *val;
  
  key_file = g_key_file_new();
  error = NULL;
    
  if(!g_key_file_load_from_file(key_file, file, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error)){
    g_debug("%s", error->message);
  }else{
    val = g_key_file_get_string(key_file, group, key, &error);    
  }

  return val;
}

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

  if (!mysql_set_character_set(cnx_init, "UTF8")){
    printf("New client character set: %s\n",
           mysql_character_set_name(cnx_init));
  }
}

void
db_connect()
{
  SERVER = config_get_string("dts.conf", "Server", "SERVER");
  g_print("Connect to: %s\n", SERVER);
  int i = 0;
  do {
    cnx_db = mysql_real_connect(cnx_init, SERVER, "orangepi_w", "0rangePi", "dts", 3306, NULL, 0);
    g_print("Connect (code 2: %d)\n", i);
  } while (cnx_db == NULL);
  /*
  if (cnx_db == NULL){
    g_print("MySQL failure to connect to database...\n");
    g_print("Exit code: 2\n");
    g_print("Error: %u -- %s\n", mysql_errno(cnx_init), mysql_error(cnx_init));
    exit(2);
  }
  */

  g_print("Database connected.\n");
}

void
db_close()
{
  mysql_close(cnx_init);
  g_print("MySQL disconnected.\n");
}

void
db_insert(gchar *sql)
{
  db_init();
  db_connect();
  if (!mysql_set_character_set(cnx_init, "UTF8")){
    printf("New cslient character set: %s\n",
           mysql_character_set_name(cnx_init));
  }

  if (mysql_query(cnx_init, sql) != 0){
    g_print("Failure to insert to database.");
    g_print("Exit code: 3\n");
    g_print("ERror: %u -- %s\n", mysql_errno(cnx_init), mysql_error(cnx_init));
    g_print("%s\n", sql);
    LOG(ERROR, "Failure to insert to database. (3)");
    exit(3);
  }
  g_print("Insert data to mysql completed.\n");
  db_close();
}

void setEntry()
{
  g_print("setEntry()\n");
  entHour = GTK_WIDGET(gtk_builder_get_object(builder, "entHour"));
  entMinute = GTK_WIDGET(gtk_builder_get_object(builder, "entMinute"));
  entDest = GTK_WIDGET(gtk_builder_get_object(builder, "entDest"));
  entRoute = GTK_WIDGET(gtk_builder_get_object(builder, "entRoute"));
  entBusNo = GTK_WIDGET(gtk_builder_get_object(builder, "entBusNo"));
  entStandard = GTK_WIDGET(gtk_builder_get_object(builder, "entStandard"));
  entPlatform = GTK_WIDGET(gtk_builder_get_object(builder, "entPlatform"));
  entNote = GTK_WIDGET(gtk_builder_get_object(builder, "entNote"));
}

void clearEntry()
{
  g_print("clearEntry()\n");
  gtk_entry_set_text(GTK_ENTRY(entHour), "");
  gtk_entry_set_text(GTK_ENTRY(entMinute), "");
  gtk_entry_set_text(GTK_ENTRY(entDest), " ");
  gtk_entry_set_text(GTK_ENTRY(entRoute), "");
  gtk_entry_set_text(GTK_ENTRY(entBusNo), "");
  gtk_entry_set_text(GTK_ENTRY(entStandard), " ");
  gtk_entry_set_text(GTK_ENTRY(entPlatform), "");
  gtk_entry_set_text(GTK_ENTRY(entNote), "");

  dts_mode = 0;
  GtkWidget *button = GTK_WIDGET(gtk_builder_get_object(builder, "btnSave"));
  gtk_button_set_label(GTK_BUTTON(button), "บันทึก");
}

void clearSelected()
{
  g_print("clearSelected()\n");
  GtkWidget *cmbDest = GTK_WIDGET(gtk_builder_get_object(builder, "cmbDest"));
  GtkWidget *cmbStandard = GTK_WIDGET(gtk_builder_get_object(builder, "cmbStandard"));

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gtk_tree_selection_unselect_all(selection);

  gtk_combo_box_set_active(GTK_COMBO_BOX(cmbDest), 0);
  gtk_combo_box_set_active(GTK_COMBO_BOX(cmbStandard), 0);

  gtk_widget_grab_focus(cmbDest);
}

G_MODULE_EXPORT
void btnDepart_clicked_cb(GtkWidget *widget, gpointer userdata)
{
  g_print("btnDepart_cliecked_cb()\n");
}

G_MODULE_EXPORT
void btnArrive_clicked_cb(GtkWidget *widget, gpointer userdata)
{
   g_print("btnArrive_clicked_cb()\n");
}

G_MODULE_EXPORT
void treeviewEvent(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
  g_print("treeviewEvent()\n");
  if (event->type == GDK_BUTTON_PRESS && event->button == 3){
    g_print("treeviewEvent: right click...\n");
    btnNewClicked(treeview, NULL);

  }

  if (event->type == GDK_BUTTON_PRESS && event->button == 1)
    g_print("treeviewEvent: left clicked...\n");

}

G_MODULE_EXPORT
void
onTreeViewRowActivated (GtkTreeView *view,
                        GtkTreePath *path,
                        GtkTreeViewColumn *col,
                        gpointer userdata)
{
  // Double-clicked on GtkTreeView //
  // ปล่อยรถ //
  GtkTreeModel *model;
  GtkTreeIter iter;

  gchar *time = 0L;
  gchar *busno = 0L;
  gchar *dest = 0L;
  gchar msg[256];

  GtkTreeSelection *selection = gtk_tree_view_get_selection(view);

  //if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(user_data), &model, &iter)){
  if (gtk_tree_selection_get_selected(selection, &model, &iter)){
    gtk_tree_model_get(model, &iter, 0, &time, -1);
    gtk_tree_model_get(model, &iter, 1, &dest, -1);
    gtk_tree_model_get(model, &iter, 2, &busno, -1);
    g_print ("ผู้ใช้คลิกปล่อยรถ: %s\n", time);
  }

  if (time != 0L){
    GtkWidget *dialog;

    g_sprintf(msg, "กรุณายืนยันปล่อยรถเที่ยวเวลา: %s\n\t\tปลายทาง: %s\n\t\tหมายเลขรถ: %s", time, dest, busno);

    dialog = gtk_message_dialog_new (GTK_WINDOW (window),
                                     GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_INFO,
                                     GTK_BUTTONS_OK_CANCEL,
                                     msg);
    //gtk_message_dialog_format_secondary_text (GTK_MESSAGE_DIALOG (dialog),
    //                                          "%d", i);
    gtk_window_set_title((GtkWindow *)dialog, "ยืนยันปล่อยรถ");
    int response = gtk_dialog_run (GTK_DIALOG (dialog));
    g_print("response was %d (OK=%d, CANCEL = %d)\n", response, GTK_RESPONSE_OK, GTK_RESPONSE_CANCEL);

    if (response == GTK_RESPONSE_OK){
      gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    }

    gtk_widget_destroy (dialog);
  }
  g_free(time);
  g_free(dest);
  g_free(busno);
}

void
treeviewSelected(GtkWidget *widget, gpointer view)
{
  g_print("treeviewSelect: clicked...\n");
  GtkTreeModel *model = gtk_tree_view_get_model(view);
  GtkTreeIter iter;

  GtkTreePath *path;
  //path = gtk_tree_model_get_path(model, &iter);
  gtk_tree_view_get_cursor(view, &path, NULL);
  g_print("signal: row-activated\n");

  if (gtk_tree_model_get_iter(model, &iter, path)){
    gchar *time;
    gchar *dest;
    gchar *busno;
    gchar *standard;
    gchar *platform;
    gchar *note;

    gtk_tree_model_get(model, &iter,
	  COL_BUSNO, &busno, COL_DEST, &dest, COL_STANDARD, &standard, COL_TIME, &time, COL_PLATFORM, &platform, COL_NOTE, &note,
      -1);

    entRoute = (GtkWidget*)gtk_builder_get_object(builder, "entRoute");
    entBusNo = (GtkWidget*)gtk_builder_get_object(builder, "entBusNo");
    entDest = (GtkWidget*)gtk_builder_get_object(builder, "entDest");
    entStandard = (GtkWidget*)gtk_builder_get_object(builder, "entStandard");
    entHour = (GtkWidget*)gtk_builder_get_object(builder, "entHour");
    entMinute = (GtkWidget*)gtk_builder_get_object(builder, "entMinute");
    entPlatform = (GtkWidget*)gtk_builder_get_object(builder, "entPlatform");
    entNote = (GtkWidget*)gtk_builder_get_object(builder, "entNote");

    gchar **arrBusNo = g_strsplit(busno, "-", 0);
    gchar **arrTime = g_strsplit(time, ":", 0);

    gtk_entry_set_text(GTK_ENTRY(entRoute), arrBusNo[0]);
    gtk_entry_set_text(GTK_ENTRY(entBusNo), arrBusNo[1]);
    gtk_entry_set_text(GTK_ENTRY(entDest), dest);
    gtk_entry_set_text(GTK_ENTRY(entStandard), standard);
    gtk_entry_set_text(GTK_ENTRY(entHour), arrTime[0]);
    gtk_entry_set_text(GTK_ENTRY(entMinute), arrTime[1]);
    gtk_entry_set_text(GTK_ENTRY(entPlatform), platform);
    gtk_entry_set_text(GTK_ENTRY(entNote), note);

    GtkWidget *button = GTK_WIDGET(gtk_builder_get_object(builder, "btnSave"));
    gtk_button_set_label(GTK_BUTTON(button), "แก้ไข");

    dts_mode = 1;

    g_print ("The row containing the time is '%s' has been clicked.\n", time);
    g_free(busno);
    g_free(dest);
    g_free(standard);
    g_free(time);
    g_free(platform);
    g_free(note);
    g_strfreev(arrBusNo);
    g_strfreev(arrTime);

    GtkWidget *entHour = GTK_WIDGET(gtk_builder_get_object(builder, "entHour"));
    GtkWidget *btnArrive = GTK_WIDGET(gtk_builder_get_object(builder, "btnArrive"));
    GtkWidget *btnDepart = GTK_WIDGET(gtk_builder_get_object(builder, "btnDepart"));
    GtkWidget *btnDelete = GTK_WIDGET(gtk_builder_get_object(builder, "btnDelete"));

    gtk_widget_set_sensitive(btnArrive, TRUE);
    gtk_widget_set_sensitive(btnDepart, TRUE);
    gtk_widget_set_sensitive(btnDelete, TRUE);
    gtk_widget_grab_focus(entHour);

    
  }
}

G_MODULE_EXPORT
void btnClicked(GtkWidget *widget, gpointer user_data)
{
  g_print("Button clicked...\n");
}

G_MODULE_EXPORT
gboolean btnSaveClicked(GtkWidget *widget, gpointer user_data)
{
  g_print("btnSave clicked...\n");

  GDateTime *now = g_date_time_new_now_local();
  gchar *curTime = g_date_time_format(now, "%Y-%m-%d %H:%M:%S");

  setEntry();

  const gchar *depHour = gtk_entry_get_text(GTK_ENTRY(entHour));
  const gchar *depMinute = gtk_entry_get_text(GTK_ENTRY(entMinute));
  const gchar *depDest = gtk_entry_get_text(GTK_ENTRY(entDest));
  const gchar *depRoute = gtk_entry_get_text(GTK_ENTRY(entRoute));
  const gchar *depBusNo = gtk_entry_get_text(GTK_ENTRY(entBusNo));
  const gchar *depStandard = gtk_entry_get_text(GTK_ENTRY(entStandard));
  const gchar *depPlatform = gtk_entry_get_text(GTK_ENTRY(entPlatform));
  const gchar *depNote = gtk_entry_get_text(GTK_ENTRY(entNote));
  
  if ( strcmp(depRoute, "") == 0 ||
       strcmp(depBusNo, "") == 0 ||
       strcmp(depHour, "") == 0 ||
       strcmp(depMinute, "") == 0 ||
       strcmp(depStandard, "") == 0)
  {
    btnNewClicked(NULL, NULL);
    g_print("\nNo data to save.\n\n");
    return TRUE; 
  }

  gchar buf_sql[256];

  if (dts_mode == 0){
    g_sprintf(buf_sql, "INSERT INTO dts_depart (dep_time, dep_dest, dep_busno, dep_standard, dep_platform, dep_note, dep_datetime) VALUES ('%s:%s', '%s', '%s-%s', '%s', '%s', '%s', '%s')", depHour, depMinute, depDest, depRoute, depBusNo, depStandard, depPlatform, depNote, curTime);

    GtkListStore *liststore = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));
    GtkTreeIter iter;

    gchar *depTime = g_strconcat(depHour, ":", depMinute, NULL);
    gchar *depBus = g_strconcat(depRoute, "-", depBusNo, NULL);

    gtk_list_store_append(liststore, &iter);
    gtk_list_store_set(
      liststore, &iter, COL_BUSNO, depBus, COL_DEST, depDest, COL_STANDARD, depStandard, COL_TIME, depTime, COL_PLATFORM, depPlatform, COL_NOTE, depNote, -1);

    g_free(depTime);
    g_free(depBus);

  }else{
    //g_sprintf(buf_sql, "UPDATE dts_depart
  }

  db_insert(buf_sql);

  g_print("%s\n", buf_sql);

  clearEntry();

  GtkWidget *cmbDest;
  cmbDest = GTK_WIDGET(gtk_builder_get_object(builder, "cmbDest"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(cmbDest), 0);

  GtkWidget *cmbStandard;
  cmbStandard = GTK_WIDGET(gtk_builder_get_object(builder, "cmbStandard"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(cmbStandard), 0);

  GtkWidget *button = GTK_WIDGET(gtk_builder_get_object(builder, "btnSave"));
  gtk_button_set_label(GTK_BUTTON(button), "บันทึก");

  gtk_widget_grab_focus(cmbDest);
  
  g_free(curTime);

  db_liststore();

  return TRUE;
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
  if (gtk_combo_box_get_active(GTK_COMBO_BOX(cmbDest)) < 0)
    return TRUE;

  entDest = GTK_WIDGET(gtk_builder_get_object(builder, "entDest"));
  entBusNo = GTK_WIDGET(gtk_builder_get_object(builder, "entBusNo"));

  gtk_combo_box_set_id_column(GTK_COMBO_BOX(cmbDest), 1);
  gtk_entry_set_text(GTK_ENTRY(entDest), gtk_combo_box_get_active_id(GTK_COMBO_BOX(cmbDest)));
  gtk_combo_box_set_id_column(GTK_COMBO_BOX(cmbDest), 0);
  gtk_entry_grab_focus_without_selecting(GTK_ENTRY(entBusNo));

  gint idx;
  idx = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
  if (idx == 0)
    gtk_widget_grab_focus(widget);

  g_print("Destination selected.\n");
  return TRUE;
}

G_MODULE_EXPORT
gboolean std_change(GtkWidget *widget, gpointer user_data)
{
  if (gtk_combo_box_get_active(GTK_COMBO_BOX(widget)) < 0)
    return TRUE;

  GtkWidget *entHour = GTK_WIDGET(gtk_builder_get_object(builder, "entHour"));
  gtk_entry_grab_focus_without_selecting(GTK_ENTRY(entHour));

  gint idx;
  idx = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
  if (idx == 0)
    gtk_widget_grab_focus(widget);

  g_print("Standard selected.\n");
  return TRUE;
}

void depart_selected(GtkWidget *widget, gpointer user_data)
{
  g_print("ยืนยันปล่อยรถเที่ยวเวลา: %s\n", (gchar*)user_data);
}

G_MODULE_EXPORT
void btnNewClicked(GtkWidget *widget, gpointer user_data)
{
  g_print("btnNew clicked\n");

  g_print("รถเข้าชานชาลา\n");
  setEntry();
  clearSelected();
  clearEntry();
  GtkWidget *entRoute = GTK_WIDGET(gtk_builder_get_object(builder, "entRoute"));
  GtkWidget *btnArrive = GTK_WIDGET(gtk_builder_get_object(builder, "btnArrive"));
  GtkWidget *btnDepart = GTK_WIDGET(gtk_builder_get_object(builder, "btnDepart"));
  GtkWidget *btnDelete = GTK_WIDGET(gtk_builder_get_object(builder, "btnDelete"));
  gtk_widget_set_sensitive(btnArrive, FALSE);
  gtk_widget_set_sensitive(btnDepart, FALSE);
  gtk_widget_set_sensitive(btnDelete, FALSE);
  gtk_widget_grab_focus(entRoute);
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

void
db_liststore()
{
  GtkListStore *store;
  GtkTreeIter iter1;
  
  db_init();
  db_connect();
  
  mysql_query(cnx_init, "SET character_set_results='utf8'");

  gchar *sql_buf = "SELECT dep_busno, dep_dest, dep_standard, dep_time, dep_platform, dep_note FROM dts_depart WHERE (STR_TO_DATE(dep_time, '%H:%i')) > (time(now() - INTERVAL 30 MINUTE)) and date(dep_datetime) = curdate();";
  
  //gchar *sql_buf = "SELECT dep_time, dep_dest, dep_busno, dep_standard, dep_platform, dep_note FROM dts_depart WHERE date(dep_datetime) = curdate();";
  
  //g_sprintf(sql_buf, "select dep_time, dep_dest, dep_busno, dep_standard, dep_platform, dep_note from dts_depart where date(dep_datetime) = '%s' and dep_depart <> 1 order by dep_time", curDate);

  
  store = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));
  
  gtk_list_store_clear(store);

  if (mysql_query(cnx_init, sql_buf) != 0L){
    g_print("query error... \n");
    g_print("ERror: %u -- %s\n", mysql_errno(cnx_init), mysql_error(cnx_init));
    LOG(ERROR, "Query error. (1)");
    LOG(INFO, sql_buf);
    exit(1);
  }
  
  // Add data from mysql to GtkListStore, store //  
  result_set = mysql_store_result(cnx_init);
  while ((row = mysql_fetch_row(result_set)) != 0L){
    gtk_list_store_append(store, &iter1);
    int n;
    for (n = 0; n < mysql_num_fields(result_set); n++){
      gtk_list_store_set(store, &iter1, n, row[n], -1);
    }
  }
  GtkTreeSortable *sortable;
  sortable = GTK_TREE_SORTABLE(store);
  gtk_tree_sortable_set_sort_column_id(sortable, 3, GTK_SORT_ASCENDING);
  
  mysql_free_result(result_set);
}

G_MODULE_EXPORT
gboolean onKeyPress(GtkWidget *widget, GdkEventKey *event, gpointer user_data){

  if (strcmp(gdk_keyval_name(event->keyval), "Return") == 0){
    g_print("press enter key.\n");
    gtk_widget_grab_focus(GTK_WIDGET(user_data));
  }
  
  return FALSE;
}


int main(int argc, char *argv[])
{
  fp= fopen("dts.log", "a+");

  GdkPixbuf *icon;
  GSList *lst, *objList;
  //GObject* window;
  GtkListStore *store_std, *store_dest;
  GtkTreeIter iter2, iter3;

  gchar home[256];
  g_sprintf(home, "%s/%s", g_get_home_dir(), "projects/dts");
  g_print("Home: %s\n", home);
  g_chdir(home);

  g_log(G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "g_log() example...");

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

  window = (GtkWidget*)gtk_builder_get_object(builder, "window");
  gtk_window_set_position(GTK_WINDOW(window), GTK_WINDOW_TOPLEVEL);
  gtk_window_set_icon(GTK_WINDOW(window), icon);
  //gtk_window_maximize(GTK_WINDOW(window));

  db_liststore();

  int i;
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
                          
  treeview = GTK_WIDGET(gtk_builder_get_object(builder, "treeview1"));
  //g_signal_connect(treeview, "button-press-event", G_CALLBACK(treeviewEvent), NULL);
  //g_signal_connect(treeview, "row-activated", G_CALLBACK(onTreeViewRowActivated), NULL);

  /*
  g_signal_connect(selection, "changed",
          G_CALLBACK(treeviewSelected), selection);
  */

  gtk_widget_show_all(GTK_WIDGET(window));

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gtk_tree_selection_unselect_all(selection);

  g_signal_connect(selection, "changed", G_CALLBACK(treeviewSelected), treeview);
  gtk_main();

  fclose(fp);

  return 0;
}
