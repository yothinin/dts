// This example demonstrates the use of a glade UI file.
//
// Note: This file must be compiled with the linker flags emitted by
// $(pkg-config gmodule-2.0) so that the dynamic loader can find the
// signal handler at runtime. See the Makefile for implementation.
#include <stdio.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include "dts.h"

GtkBuilder* builder;
GtkWidget *window;
GtkWidget *treeview;
GtkTreeSelection *selection;

GtkWidget *entHour, *entMinute;
GtkWidget *entDest, *entRoute, *entBusNo;
GtkWidget *entStandard;
GtkWidget *entPlatform;
GtkWidget *entNote;
GtkWidget *cmbDest;
GtkWidget *cmbStandard;
GtkWidget *btnDepart;
GtkWidget *btnArrive;
GtkWidget *btnEmpty;
GtkWidget *btnDelete;

gint dts_mode = 0; /* 0 = Insert, 1 = Update, 2 = NONE for liststore. */
gint handler_select;
gchar *SERVER;

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
  COL_STANDARD,
  COL_DEST,
  COL_TIME,
  COL_PLATFORM,
  COL_STATUS,
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
  {"91", "ลำปาง"},
  {"909", "เชียงราย"},
  {"910", "น่าน"},
  {"922", "พะเยา"},
  {"923", "แพร่"},
  {"928", "มุกดาหาร"},
  {"933", "ศรีเชียงใหม่"},
  {"940", "ศรีสะเกษ"},
  {"957", "แม่สาย"},
  {"961", "แม่ฮ่องสอน"},
  {"962", "เชียงของ"},
};

struct MemoryStruct{
  char *memory;
  size_t size;
};

struct MemoryStruct chunk;

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
  mem->memory = realloc(mem->memory, mem->size+realsize + 1);

  if (mem->memory == NULL){
    // out of memory
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;

}

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
  GtkWidget *entRoute = GTK_WIDGET(gtk_builder_get_object(builder, "entRoute"));
  GtkWidget *entBusNo = GTK_WIDGET(gtk_builder_get_object(builder, "entBusNo"));
  GtkWidget *cmbDest = GTK_WIDGET(gtk_builder_get_object(builder, "cmbDest"));
  GtkWidget *cmbStandard = GTK_WIDGET(gtk_builder_get_object(builder, "cmbStandard"));

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gtk_tree_selection_unselect_all(selection);

  gtk_entry_set_text(GTK_ENTRY(entRoute), "");
  gtk_entry_set_text(GTK_ENTRY(entBusNo), "");
  gtk_combo_box_set_active(GTK_COMBO_BOX(cmbDest), 0);
  gtk_combo_box_set_active(GTK_COMBO_BOX(cmbStandard), 0);

  gtk_widget_grab_focus(cmbDest);
}

G_MODULE_EXPORT
void btnEmpty_clicked_cb(GtkWidget *widget, gpointer userdata)
{
  GtkTreeIter iter;
  GtkListStore *store;
  gchar *depStdCode;
  GtkComboBox *cmbStandard = GTK_COMBO_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "cmbStandard")));
  gtk_combo_box_get_active_iter(cmbStandard, &iter);
  store = GTK_LIST_STORE(gtk_combo_box_get_model(cmbStandard));
  gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 0, &depStdCode, -1);

  entRoute = (GtkWidget*)gtk_builder_get_object(builder, "entRoute");
  entBusNo = (GtkWidget*)gtk_builder_get_object(builder, "entBusNo");
  const gchar *depRoute = gtk_entry_get_text(GTK_ENTRY(entRoute));
  const gchar *depBusNo = gtk_entry_get_text(GTK_ENTRY(entBusNo));

  GDateTime *now = g_date_time_new_now_local();
  gchar *curDate = g_date_time_format(now, "%Y-%m-%d");
  gchar *url = "https://dts.bustecz.com/dts_api/busclear.php";
  gchar *posData;
  posData = g_strconcat(
    "depBusno=",    depRoute  , "-", depBusNo, "&",
    "depStdCode=",  depStdCode, "&",
    "depDatetime=", curDate   ,
    NULL
  );
  ///g_print("POST: %s\n", posData);
  ///g_print("URL : %s\n", url);

  if (execApi(url, posData) == 0){
    g_print("Clear: %s, %s-%s, %s\n", curDate, depRoute, depBusNo, depStdCode);
  }

  db_liststore();
  btnNewClicked(NULL, NULL);

}

G_MODULE_EXPORT
void btnDepart_clicked_cb(GtkWidget *widget, gpointer userdata)
{
  //g_print("btnDepart_cliecked_cb()\n");
  GtkTreeIter iter;
  GtkListStore *store;
  gchar *depStdCode;
  GtkComboBox *cmbStandard = GTK_COMBO_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "cmbStandard")));
  gtk_combo_box_get_active_iter(cmbStandard, &iter);
  store = GTK_LIST_STORE(gtk_combo_box_get_model(cmbStandard));
  gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 0, &depStdCode, -1);

  entRoute = (GtkWidget*)gtk_builder_get_object(builder, "entRoute");
  entBusNo = (GtkWidget*)gtk_builder_get_object(builder, "entBusNo");
  const gchar *depRoute = gtk_entry_get_text(GTK_ENTRY(entRoute));
  const gchar *depBusNo = gtk_entry_get_text(GTK_ENTRY(entBusNo));

  GDateTime *now = g_date_time_new_now_local();
  gchar *curDate = g_date_time_format(now, "%Y-%m-%d");
  gchar *url = "https://dts.bustecz.com/dts_api/busdepart.php";
  gchar *posData;
  posData = g_strconcat(
    "depBusno=",    depRoute  , "-", depBusNo, "&",
    "depStdCode=",  depStdCode, "&",
    "depDatetime=", curDate   ,
    NULL
  );
  
  //g_print("%s\n", posData);
  //g_print(url);

  if (execApi(url, posData) == 0){
    g_print("Depart: %s, %s-%s, %s\n", curDate, depRoute, depBusNo, depStdCode);
  }

  db_liststore();
  btnNewClicked(NULL, NULL);

}

G_MODULE_EXPORT
void btnRefresh_clicked_cb (GtkWidget *widget, gpointer user_data)
{
  g_print("btnRefresh_clicked_cb::clicked\n");
  db_liststore();
}

G_MODULE_EXPORT
void btnArrive_clicked_cb(GtkWidget *widget, gpointer userdata)
{
  //g_print("btnArrive_clicked_cb()\n");

  GtkTreeView *treeview = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview1"));
  selection = gtk_tree_view_get_selection(treeview);
  gtk_tree_selection_unselect_all(selection);
  
  GtkTreeIter iter;
  GtkListStore *store;
  gchar *depStdCode;
  GtkComboBox *cmbStandard = GTK_COMBO_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "cmbStandard")));
  gtk_combo_box_get_active_iter(cmbStandard, &iter);
  store = GTK_LIST_STORE(gtk_combo_box_get_model(cmbStandard));
  gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 0, &depStdCode, -1);

  entRoute = (GtkWidget*)gtk_builder_get_object(builder, "entRoute");
  entBusNo = (GtkWidget*)gtk_builder_get_object(builder, "entBusNo");
  const gchar *depRoute = gtk_entry_get_text(GTK_ENTRY(entRoute));
  const gchar *depBusNo = gtk_entry_get_text(GTK_ENTRY(entBusNo));

  GDateTime *now = g_date_time_new_now_local();
  gchar *curDate = g_date_time_format(now, "%Y-%m-%d");

  gchar *url = "https://dts.bustecz.com/dts_api/busarrive.php";
  gchar *posData;
  posData = g_strconcat(
    "depBusno=",    depRoute  , "-", depBusNo, "&",
    "depStdCode=",  depStdCode, "&",
    "depDatetime=", curDate   ,
    NULL
  );
  ///g_print("%s\n", posData);
  ///g_print(url);

  if (execApi(url, posData) == 0){
    g_print("Arrive: %s, %s-%s, %s\n", curDate, depRoute, depBusNo, depStdCode);
  }

  db_liststore();
  btnNewClicked(NULL, NULL);

}

G_MODULE_EXPORT
void
TreeViewSelect(GtkWidget *widget, gpointer view)
{
  g_print("Treeview select...\n");
}

void
treeviewSelected(GtkWidget *widget, gpointer view)
{
  g_print("treeviewSelected: clicked...\n");
  //g_print("signal: row-activated\n");
  //g_print ("The row containing the time is '%s' has been clicked.\n", time);

  GtkTreeModel *model = gtk_tree_view_get_model(view);
  GtkTreeIter iter;

  GtkTreePath *path;
  //path = gtk_tree_model_get_path(model, &iter);
  gtk_tree_view_get_cursor(view, &path, NULL);

  if (gtk_tree_model_get_iter(model, &iter, path)){
    gchar *time, *dest, *busno, *standard;
    gchar *platform, *status, *note;

    gtk_tree_model_get(model, &iter,
      COL_BUSNO, &busno, COL_DEST, &dest, COL_STANDARD, &standard, COL_TIME, &time, COL_PLATFORM, &platform, COL_STATUS, &status, COL_NOTE, &note,
      -1);

    entRoute = (GtkWidget*)gtk_builder_get_object(builder, "entRoute");
    entBusNo = (GtkWidget*)gtk_builder_get_object(builder, "entBusNo");
    entDest = (GtkWidget*)gtk_builder_get_object(builder, "entDest");
    entStandard = (GtkWidget*)gtk_builder_get_object(builder, "entStandard");
    entHour = (GtkWidget*)gtk_builder_get_object(builder, "entHour");
    entMinute = (GtkWidget*)gtk_builder_get_object(builder, "entMinute");
    entPlatform = (GtkWidget*)gtk_builder_get_object(builder, "entPlatform");
    entNote = (GtkWidget*)gtk_builder_get_object(builder, "entNote");
    cmbDest = (GtkWidget*)gtk_builder_get_object(builder, "cmbDest");
    cmbStandard = (GtkWidget*)gtk_builder_get_object(builder, "cmbStandard");

    gtk_widget_set_sensitive(entRoute, FALSE);
    gtk_widget_set_sensitive(entBusNo, FALSE);
    gtk_widget_set_sensitive(entDest, FALSE);
    gtk_widget_set_sensitive(entStandard, FALSE);
    gtk_widget_set_sensitive(cmbDest, FALSE);
    gtk_widget_set_sensitive(cmbStandard, FALSE);

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

    g_free(busno);
    g_free(dest);
    g_free(standard);
    g_free(time);
    g_free(platform);
    g_free(note);
    g_strfreev(arrBusNo);
    g_strfreev(arrTime);

    GtkWidget *entHour   = GTK_WIDGET(gtk_builder_get_object(builder, "entHour"));
    GtkWidget *btnArrive = GTK_WIDGET(gtk_builder_get_object(builder, "btnArrive"));
    GtkWidget *btnDepart = GTK_WIDGET(gtk_builder_get_object(builder, "btnDepart"));
    GtkWidget *btnEmpty = GTK_WIDGET(gtk_builder_get_object(builder, "btnEmpty"));
    GtkWidget *btnDelete = GTK_WIDGET(gtk_builder_get_object(builder, "btnDelete"));

    gtk_widget_set_sensitive(btnArrive, TRUE);
    gtk_widget_set_sensitive(btnDepart, TRUE);
    gtk_widget_set_sensitive(btnEmpty, TRUE);
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

  GtkTreeIter iter;
  GtkListStore *store;

  gchar *depStdCode;

  GtkComboBox *cmbStandard = GTK_COMBO_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "cmbStandard")));
  gtk_combo_box_get_active_iter(cmbStandard, &iter);
  store = GTK_LIST_STORE(gtk_combo_box_get_model(cmbStandard));
  gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 0, &depStdCode, -1);

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

  gchar *url;
  if (dts_mode == 0)
    url = "https://dts.bustecz.com/dts_api/addsch.php";
  else
    url = "https://dts.bustecz.com/dts_api/updsch.php";

  gchar *postData = g_strconcat(
                      "depTime=",     depHour,     ":", depMinute, "&",
                      "depDest=",     depDest,     "&",
                      "depBusno=",    depRoute,    "-", depBusNo,  "&",
                      "depStdCode=",  depStdCode,  "&",
                      "depStandard=", depStandard, "&",
                      "depPlatform=", depPlatform, "&",
                      "depNote=",     depNote,     "&",
                      "depDepart=",   "0",
                      NULL
                    );

  g_print("url=%s\nposData=%s\n", url, postData);

  if (execApi(url, postData) == 0)
    g_print("Status: completed.\n");
    ////g_print("Staus = %s\n", execJson("Status")";

  g_free(postData);

  clearEntry();

  GtkWidget *cmbDest;
  cmbDest = GTK_WIDGET(gtk_builder_get_object(builder, "cmbDest"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(cmbDest), 0);
  gtk_combo_box_set_active(GTK_COMBO_BOX(cmbStandard), 0);

  GtkWidget *button = GTK_WIDGET(gtk_builder_get_object(builder, "btnSave"));
  gtk_button_set_label(GTK_BUTTON(button), "บันทึก");
  gtk_widget_grab_focus(cmbDest);

  g_free(curTime);

  db_liststore();
  btnNewClicked(NULL, NULL);

  return TRUE;
}

G_MODULE_EXPORT
void btnDelete_clicked_cb(GtkWidget *widget, gpointer user_data)
{
  g_print("btnDelete clicked...\n");
  GtkTreeIter iter;
  GtkListStore *store;
  gchar *depStdCode;
  GtkComboBox *cmbStandard = GTK_COMBO_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "cmbStandard")));
  gtk_combo_box_get_active_iter(cmbStandard, &iter);
  store = GTK_LIST_STORE(gtk_combo_box_get_model(cmbStandard));
  gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 0, &depStdCode, -1);

  entRoute = (GtkWidget*)gtk_builder_get_object(builder, "entRoute");
  entBusNo = (GtkWidget*)gtk_builder_get_object(builder, "entBusNo");
  const gchar *depRoute = gtk_entry_get_text(GTK_ENTRY(entRoute));
  const gchar *depBusNo = gtk_entry_get_text(GTK_ENTRY(entBusNo));

  GDateTime *now = g_date_time_new_now_local();
  gchar *curDate = g_date_time_format(now, "%Y-%m-%d");
  gchar *url = "https://dts.bustecz.com/dts_api/delsch.php";
  gchar *posData;
  posData = g_strconcat(
    "depBusno=",    depRoute  , "-", depBusNo, "&",
    "depStdCode=",  depStdCode, "&",
    "depDatetime=", curDate   ,
    NULL
  );

  g_print("%s\n", posData);

  if (execApi(url, posData) == 0)
    g_print("Deleted: %s, %s-%s, %s\n", curDate, depRoute, depBusNo, depStdCode);

  db_liststore();
  btnNewClicked(NULL, NULL);
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

G_MODULE_EXPORT
void std_buscheck(GtkWidget *widget, gpointer userdata)
{
  g_print("FUNCTION: chk_change()\n");
  GtkTreeIter iter;
  GtkListStore *store;
  gchar *depStdCode;
  GtkComboBox *cmbStandard = GTK_COMBO_BOX(GTK_WIDGET(gtk_builder_get_object(builder, "cmbStandard")));
  gtk_combo_box_get_active_iter(cmbStandard, &iter);
  store = GTK_LIST_STORE(gtk_combo_box_get_model(cmbStandard));
  gtk_tree_model_get(GTK_TREE_MODEL(store), &iter, 0, &depStdCode, -1);

  entRoute = (GtkWidget*)gtk_builder_get_object(builder, "entRoute");
  entBusNo = (GtkWidget*)gtk_builder_get_object(builder, "entBusNo");
  const gchar *depRoute = gtk_entry_get_text(GTK_ENTRY(entRoute));
  const gchar *depBusNo = gtk_entry_get_text(GTK_ENTRY(entBusNo));

  GDateTime *now = g_date_time_new_now_local();
  gchar *curDate = g_date_time_format(now, "%Y-%m-%d");
  gchar *url = "https://dts.bustecz.com/dts_api/getbus.php";
  gchar *posData;
  posData = g_strconcat(
    "depBusno=",    depRoute  , "-", depBusNo, "&",
    "depStdCode=",  depStdCode, "&",
    "depDatetime=", curDate   ,
    NULL
  );

  if ( strcmp(depRoute, "") == 0 ||
       strcmp(depBusNo, "") == 0 ||
       strcmp(depStdCode, "") == 0)
    g_print("No bus to check.\n");
  else{
    g_print("std_buscheck: %s\n", posData);
    g_print("url: %s\n", url);
    if (execApi(url, posData) == 0){
      //g_print("Data: %s\n", chunk.memory);
      /* Set mode to edit and change btnSave label to edit. */

      json_object *root = json_tokener_parse(chunk.memory);
      int i;
      int n = json_object_array_length(root);
      
      if (n == 0)
        dts_mode = 0;
      else
        dts_mode = 1;

      gtk_button_set_label(GTK_BUTTON(GTK_WIDGET(gtk_builder_get_object(builder, "btnSave"))), (dts_mode == 0)?"บันทึก":"แก้ไข");
      
      for (i = 0; i<n; i++){
        const char *str = json_object_get_string(json_object_array_get_idx(root, i));
        g_print("String: %s\n", str);
        json_object *sch = json_tokener_parse(str);
        json_object *objTime, *objPlatform, *objNote;

        json_object_object_get_ex(sch, "dep_time", &objTime);
        json_object_object_get_ex(sch, "dep_platform", &objPlatform);
        json_object_object_get_ex(sch, "dep_note", &objNote);

        gchar **tt;
        tt = g_strsplit(json_object_get_string(objTime), ":", 0);

        g_print("Time: %s-%s, Plaform: %s, Note: %s\n", tt[0], tt[1],
                                                   json_object_get_string(objPlatform),
                                                   json_object_get_string(objNote));

        gtk_entry_set_text(GTK_ENTRY(entHour), tt[0]);
        gtk_entry_set_text(GTK_ENTRY(entMinute), tt[1]);
        gtk_entry_set_text(GTK_ENTRY(entPlatform), json_object_get_string(objPlatform));
        gtk_entry_set_text(GTK_ENTRY(entNote), json_object_get_string(objNote));

        g_strfreev (tt);

        json_object_put(objTime);
        json_object_put(objPlatform);
        json_object_put(objNote);
      }
    }
  }
}

G_MODULE_EXPORT
void btnNewClicked(GtkWidget *widget, gpointer user_data)
{
  g_print("btnNew clicked\n");
  setEntry();
  clearSelected();
  clearEntry();

  entRoute = GTK_WIDGET(gtk_builder_get_object(builder, "entRoute"));
  btnArrive = GTK_WIDGET(gtk_builder_get_object(builder, "btnArrive"));
  btnDepart = GTK_WIDGET(gtk_builder_get_object(builder, "btnDepart"));
  btnEmpty = GTK_WIDGET(gtk_builder_get_object(builder, "btnEmpty"));
  btnDelete = GTK_WIDGET(gtk_builder_get_object(builder, "btnDelete"));

  cmbDest = GTK_WIDGET(gtk_builder_get_object(builder, "cmbDest"));
  cmbStandard = GTK_WIDGET(gtk_builder_get_object(builder, "cmbStandard"));

  gtk_widget_set_sensitive(entRoute, TRUE);
  gtk_widget_set_sensitive(entBusNo, TRUE);
  gtk_widget_set_sensitive(entDest, TRUE);
  gtk_widget_set_sensitive(entStandard, TRUE);
  gtk_widget_set_sensitive(cmbDest, TRUE);
  gtk_widget_set_sensitive(cmbStandard, TRUE);

  gtk_widget_set_sensitive(btnArrive, FALSE);
  gtk_widget_set_sensitive(btnDepart, FALSE);
  gtk_widget_set_sensitive(btnEmpty, FALSE);
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

int execApi(char *url, char *postData)
{
  CURL *curl;
  CURLcode res;

  //struct MemoryStruct chunk;
  chunk.memory = malloc(1);
  chunk.size = 0;

  curl_global_init(CURL_GLOBAL_ALL);
  curl = curl_easy_init();

  if (curl){
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    res = curl_easy_perform(curl);
    g_printf("%s\n", chunk.memory);
  }

  return res;
}

void
db_liststore()
{
  dts_mode = 2;
  GtkListStore *store;
  GtkTreeIter iter1;
  store = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));

  GtkWidget *treeview = GTK_WIDGET(gtk_builder_get_object(builder, "treeview1"));
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  
  if (handler_select > 0)
    g_signal_handler_disconnect(selection, handler_select);
  
  gtk_list_store_clear(store);
  handler_select = g_signal_connect(selection, "changed", G_CALLBACK(treeviewSelected), treeview);

  GtkTreeSortable *sortable;
  sortable = GTK_TREE_SORTABLE(store);
  gtk_tree_sortable_set_sort_column_id(sortable, 3, GTK_SORT_ASCENDING);

  char *url = "https://dts.bustecz.com/dts_api/getsch-all.php";
  char *postData = "";

  /* call execApi() function and return res. */
  if (execApi(url, postData) == CURLE_OK){
    json_object *root = json_tokener_parse(chunk.memory);
    int i;
    int n = json_object_array_length(root);
    for (i = 0; i<n; i++){
      const char *str = json_object_get_string(json_object_array_get_idx(root, i));
      g_print(str);
      json_object *sch = json_tokener_parse(str);
      json_object *objTime, *objDest, *objBusno, *objStandard, *objPlatform, *objDepart, *objNote;

      json_object_object_get_ex(sch, "dep_time", &objTime);
      json_object_object_get_ex(sch, "dep_dest", &objDest);
      json_object_object_get_ex(sch, "dep_busno", &objBusno);
      json_object_object_get_ex(sch, "dep_standard", &objStandard);
      json_object_object_get_ex(sch, "dep_platform", &objPlatform);
      json_object_object_get_ex(sch, "dep_depart", &objDepart);
      json_object_object_get_ex(sch, "dep_note", &objNote);

      gtk_list_store_append(store, &iter1);
      gtk_list_store_set(store, &iter1,
        0, json_object_get_string(objBusno),
        1, json_object_get_string(objStandard),
        2, json_object_get_string(objDest),
        3, json_object_get_string(objTime),
        4, json_object_get_string(objPlatform),
        5, json_object_get_string(objDepart),
        6, json_object_get_string(objNote),
        -1
      );

      json_object_put(objTime);
      json_object_put(objDest);
      json_object_put(objBusno);
      json_object_put(objStandard);
      json_object_put(objPlatform);
      json_object_put(objDepart);
      json_object_put(objNote);

    }
    json_object_put(root);
  }
}

G_MODULE_EXPORT
gboolean onKeyPress(GtkWidget *widget, GdkEventKey *event, gpointer user_data){

  if (strcmp(gdk_keyval_name(event->keyval), "Return") == 0){
    g_print("press enter key.\n");
    gtk_widget_grab_focus(GTK_WIDGET(user_data));
    return TRUE;
  }

  return FALSE;
}


int main(int argc, char *argv[])
{
  GdkPixbuf *icon;
  GtkListStore *store_std, *store_dest;
  GtkTreeIter iter2, iter3;

  gchar home[256];
  g_sprintf(home, "%s/%s", g_get_home_dir(), "projects/dts");
  g_print("Home: %s\n", home);
  g_chdir(home);

  icon = create_pixbuf("time-mgr.png");

  gtk_init(&argc, &argv);

  ///builder = gtk_builder_new();
  ///gtk_builder_add_from_file(builder, "ex_tree.glade", NULL);
  builder = gtk_builder_new_from_file("ex_tree.glade");

  //builder = gtk_builder_new_from_resource("/com/bustecz/dts/ex_from_glade.xml");
  gtk_builder_connect_signals(builder, NULL);

  //GSList *lst, *objList;
  //objList = gtk_builder_get_objects(builder);
  //g_print("%p\n", objList);
  //for (lst = objList; lst != NULL; lst = lst->next){
    //g_print("%p\n", (char*)(lst->data));
  //}

  window = (GtkWidget*)gtk_builder_get_object(builder, "window");
  gtk_window_set_position(GTK_WINDOW(window), GTK_WINDOW_TOPLEVEL);
  gtk_window_set_icon(GTK_WINDOW(window), icon);
  gtk_window_maximize(GTK_WINDOW(window));
  //gtk_window_fullscreen(GTK_WINDOW(window));
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

  gtk_widget_show_all(GTK_WIDGET(window));

  ///selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  ///gtk_tree_selection_unselect_all(selection);

  /* Change to set g_signal_connect to db_liststore() */
  // handler_select = g_signal_connect(selection, "changed", G_CALLBACK(treeviewSelected), treeview);
  gtk_main();

  return 0;
}
