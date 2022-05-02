/*
 * gcc -g -Wall -o dts-simple dts-simple.c dts_functions.c `pkg-config --libs --cflags gtk+-2.0` `mysql_config --cflags --libs` -DGTK_DISABLE_DEPRECATE
 * 
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include <mysql.h>
#include <gdk/gdk.h>
#include "dts_functions.h"

#define LOG(X, Y) fprintf (fp, #X ": Time:%s, File:%s(%d) " #Y  "\n", __TIMESTAMP__, __FILE__, __LINE__)
FILE *fp;

  gint threadID = 0;

  GtkWidget *win=NULL;

  // Read file dts.conf to setup color, font and font size.
  gchar *SERVER;
  gchar *BG_IMAGE;
  gchar *TITLE_TEXT;
  gchar *TITLE_FONT;
  gint TITLE_SIZE;
  gchar *HEADER_COLOR;
  gchar *TIME_COLOR;
  gchar *CONTENT_COLOR;
  gchar *FONT_SIZE_F;
  gint FONT_SIZE;

typedef struct
{
  const gchar 	  *time;
  const gchar		  *dest;
  const gchar		  *busno;
  const gchar		  *standard;
  const gchar		  *platform;
  const gchar		  *note;
  const gboolean	out;
  const gchar		  *objname;
}
Table;

MYSQL *cnx_init;
MYSQL *cnx_db;
MYSQL_RES *result_set;
MYSQL_ROW row;

void
db_init()
{
  g_print("Start connect to sql...\n");
  cnx_init = mysql_init(NULL);
  if (cnx_init == NULL){
    g_print("connect failed in mysql_init, \n");
    g_print("exit code: 1\n");
    LOG(ERROR, "Initialize failed.");
    exit(1);
  }else{
    bool reconnect = 0;
    unsigned int timeout = 1;
    mysql_options(cnx_init, MYSQL_OPT_RECONNECT, &reconnect);
    mysql_options(cnx_init, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    
  }
}

void
db_connect()
{
  int i = 0;
  
  do {
    g_print("LOST CONNECTION...(%d)\n", i++);
    cnx_db = mysql_real_connect(cnx_init, SERVER, "orangepi_r", "0rangePi", "dts", 3306, NULL, 0);
  } while (cnx_db == NULL);

}

void
db_close()
{
  mysql_close(cnx_init);
  g_print("MySQL disconnected.\n");
  LOG(INFO, "MySQL disconnected.");
}

static gboolean
update_time(GtkWidget *label)
{
  GDateTime *now = g_date_time_new_now_local();
  gchar *mytime = g_date_time_format(now, "%d-%m-%y, %H:%M:%S");
  gchar buf[256];

  g_sprintf(buf, "%s", mytime);
  g_free(mytime);
  g_date_time_unref(now);
  
  gtk_label_set_text(GTK_LABEL(label), buf);

  return TRUE;
}

static gboolean
displayLabel (GtkWidget *widget)
{
  g_source_remove(threadID);
 
  db_init();
  db_connect();

  //mysql_query(cnx_init, "SET character_set_results='utf8'");
  if (!mysql_set_character_set(cnx_init, "utf8"))
  {
    printf("New client character set: %s\n", mysql_character_set_name(cnx_init));
  }
  //gchar *sql_buf = "SELECT dep_time, dep_dest, dep_busno, dep_standard, dep_platform, (CASE WHEN dep_depart = 0 THEN ' ' WHEN dep_depart = 1 THEN 'เข้าชานชาลา' WHEN dep_depart = 2 THEN 'รถออก' END) as dep_status FROM dts_depart WHERE (STR_TO_DATE(dep_time, '%H:%i')) > (time(now() - INTERVAL 30 MINUTE)) and date(dep_datetime) = curdate() order by dep_time limit 0,9;";
  //            "  (CASE ",
  //            "    WHEN dep_depart = 0 THEN ' ' ",
  //            "    WHEN dep_depart = 1 THEN 'เข้า' ",
  //            "    WHEN dep_depart = 2 THEN 'ออก' ",
  //            "  END) as dep_status ",


  gchar *sql_buf;
  sql_buf = g_strconcat(
              "select ",
              "  dep_time, dep_dest, dep_busno, dep_standard, dep_platform, dep_depart ", 
              "FROM ",
              "  dts_depart ",
              "WHERE ",
              "  DATE_FORMAT(CONCAT(dep_date, ' ', dep_time), '%Y-%m-%d %T') > ",
              "  DATE_FORMAT(now()-interval 20 minute, '%Y-%m-%d %T') AND ",
              "  dep_date = DATE(curdate()) ",
              "ORDER BY dep_date, dep_time ", NULL);
  
  if (mysql_query(cnx_init, sql_buf) != 0L){
    g_print("query error... \n");
    g_print("ERror: %u -- %s\n", mysql_errno(cnx_init), mysql_error(cnx_init));
    LOG(ERROR, "Querry error.");
    //exit(1);
  }
  LOG(INFO, "Query in displayLabel()");
  // Add data from mysql to GtkListStore, store //  
  result_set = mysql_store_result(cnx_init);
  
  gtk_container_foreach(GTK_CONTAINER(widget), (void*) gtk_widget_destroy, NULL);

  GtkWidget *lbl=NULL;
  GtkWidget *hbox_c;

  while ((row = mysql_fetch_row(result_set)) != 0L){
	gchar *depStatus;
	if (*row[5] == '1')
	  depStatus = "เข้า";
	else if (*row[5] == '2')
	  depStatus = "ออก";
	else
	  depStatus = "";
	  
    hbox_c = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(widget), hbox_c, FALSE, FALSE, 0);
    set_label(hbox_c, lbl, 5, row[0], FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);
    set_label(hbox_c, lbl, 10, row[1], FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);
    set_label(hbox_c, lbl, 7, row[2], FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);
    set_label(hbox_c, lbl, 8, row[3], FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);
    set_label(hbox_c, lbl, 7, row[4], FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);
    set_label(hbox_c, lbl, 15, depStatus, FONT_SIZE, (*row[5] == '1')?CONTENT_COLOR:"#ff0000", FALSE, FALSE);
  }

  gtk_widget_show_all(win);
  g_print("Server: %s,\n%s\n", SERVER, sql_buf);
  mysql_free_result(result_set);
  db_close();

  threadID = g_timeout_add (10000, (GSourceFunc)displayLabel, widget);

  return TRUE;
}

int main(int argc, char *argv[]){
  GdkPixbuf *icon;
  GtkWidget *vbox, *vbox_c;
  GtkWidget *hbox;
  GtkWidget *lblDateTime;
  GtkWidget *lblName = NULL;
  GtkWidget *lblHeader = NULL;
  PangoFontDescription *dfDateTime;
  PangoFontDescription *dfName;
  
  GdkPixbuf *image = NULL;
  GdkPixmap *background = NULL;
  GtkStyle *style = NULL;
  GdkColor NameColor;
  GdkColor TimeColor;
  
  gchar home[256];
  g_sprintf(home, "%s/%s", g_get_home_dir(), "projects/dts");
  g_print("Home: %s\n", home);
  g_chdir(home);

  if (remove("dts.log") == 0)
    g_print("Delete file dts.log successfully.\n");
  fp= fopen("dts.log", "a+");
  fprintf (fp, "Current dir: %s\n", home);

  gtk_init(&argc, &argv);
  
  // Read file dts.conf to setup color, font and font size.
  SERVER = config_get_string("dts.conf", "Server", "SERVER");
  BG_IMAGE = config_get_string("dts.conf", "Images", "BG_IMAGE");
  TITLE_TEXT = config_get_string("dts.conf", "Title", "TITLE_TEXT");
  TITLE_FONT = config_get_string("dts.conf", "Title", "TITLE_FONT");
  TITLE_SIZE = config_get_integer("dts.conf", "Title", "TITLE_SIZE");
  HEADER_COLOR = config_get_string("dts.conf", "Color", "HEADER_COLOR");
  TIME_COLOR = config_get_string("dts.conf", "Color", "TIME_COLOR");
  CONTENT_COLOR = config_get_string("dts.conf", "Color", "CONTENT_COLOR");
  FONT_SIZE_F = config_get_string("dts.conf", "Contents", "FONT_SIZE");
  FONT_SIZE = config_get_integer("dts.conf", "Contents", FONT_SIZE_F);

  image = load_pixbuf_from_file (BG_IMAGE);
  gdk_pixbuf_render_pixmap_and_mask (image, &background, NULL, 0);
  style = gtk_style_new ();
  style->bg_pixmap [0] = background;
  
  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(win), "Schedule Panel");
  gtk_window_set_default_size(GTK_WINDOW(win), 400, 600);
  gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);

  gtk_widget_set_style (GTK_WIDGET(win), GTK_STYLE (style));
  gtk_window_set_transient_for (GTK_WINDOW (win), NULL);
 
  gtk_window_fullscreen(GTK_WINDOW(win));

  icon = create_pixbuf("Digital-Signage.png");
  gtk_window_set_icon(GTK_WINDOW(win), icon);

  vbox = gtk_vbox_new(FALSE, 1);
  gtk_container_add(GTK_CONTAINER(win), vbox);
  
  hbox = gtk_hbox_new(FALSE, 1);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  
  lblName = gtk_label_new(TITLE_TEXT);
  lblDateTime = gtk_label_new("");

  dfDateTime = pango_font_description_from_string(TITLE_FONT);
  pango_font_description_set_size(dfDateTime, TITLE_SIZE*PANGO_SCALE);
  
  dfName = pango_font_description_from_string(TITLE_FONT);
  pango_font_description_set_size(dfName, TITLE_SIZE*PANGO_SCALE);
  pango_font_description_set_weight(dfName, PANGO_WEIGHT_BOLD);

  gdk_color_parse(HEADER_COLOR, &NameColor);
  gdk_color_parse(TIME_COLOR, &TimeColor);
  gtk_widget_modify_fg(GTK_WIDGET(lblName), GTK_STATE_NORMAL, &NameColor);
  gtk_widget_modify_fg(GTK_WIDGET(lblDateTime), GTK_STATE_NORMAL, &TimeColor);

  gtk_widget_modify_font(lblDateTime, dfDateTime);
  gtk_widget_modify_font(lblName, dfName);
  gtk_box_pack_start(GTK_BOX(hbox), lblName, FALSE, FALSE, 10);
  gtk_box_pack_end(GTK_BOX(hbox), lblDateTime, FALSE, FALSE, 20);
  
  //header
  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE,0);
  set_label(hbox, lblHeader, 5, "เวลา", FONT_SIZE, HEADER_COLOR, TRUE, TRUE);
  set_label(hbox, lblHeader, 10, "ปลายทาง", FONT_SIZE, HEADER_COLOR, TRUE, TRUE);
  set_label(hbox, lblHeader, 7, "เลขรถ", FONT_SIZE, HEADER_COLOR, TRUE, TRUE);
  set_label(hbox, lblHeader, 8, "มาตรฐาน", FONT_SIZE, HEADER_COLOR, TRUE, TRUE);
  set_label(hbox, lblHeader, 7, "ชานชาลา", FONT_SIZE, HEADER_COLOR, TRUE, TRUE);
  set_label(hbox, lblHeader, 15, "สถานะ", FONT_SIZE, HEADER_COLOR, TRUE, TRUE);

  vbox_c = gtk_vbox_new(FALSE, 1);
  gtk_box_pack_start(GTK_BOX(vbox), vbox_c, FALSE, FALSE,0);
  
  //displayLabel(vbox_c);
  
  g_timeout_add (1000, (GSourceFunc)update_time, lblDateTime);
  threadID = g_timeout_add (5000, (GSourceFunc)displayLabel, vbox_c);

  g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_realize(win);

  GdkCursor *cursor = gdk_cursor_new(GDK_BLANK_CURSOR);
  GdkWindow *w = gtk_widget_get_window(GTK_WIDGET(win));
  if (w != NULL)
    gdk_window_set_cursor(w, cursor);

  gtk_widget_show_all(win);
  gtk_main();

  fprintf(fp, "Normal ending program.\n");
  fclose(fp);

  return 0;
}
