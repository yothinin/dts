/*
 * gcc -g -Wall -o dts-simple dts-simple.c dts_functions.c `pkg-config --libs --cflags gtk+-2.0` `mysql_config --cflags --libs` -DGTK_DISABLE_DEPRECATE
 *
 */

#include <gtk/gtk.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>
#include <gdk/gdk.h>
#include <time.h>
#include <curl/curl.h>
#include <json-c/json.h>

#include "dts_functions.h"

//FILE *fp;

gint threadID = 0;

GtkWidget *win=NULL;

// Read file dts.conf to setup color, font and font size.
gchar *SERVER;
gchar *BG_IMAGE;
gchar *TITLE_TEXT;
gchar *TITLE_FONT;
gint   TITLE_SIZE;
gchar *HEADER_COLOR;
gchar *TIME_COLOR;
gchar *CONTENT_COLOR;
gchar *FONT_SIZE_F;
gint   FONT_SIZE;

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

struct MemoryStruct{
  char *memory;
  size_t size;
};

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

  time_t now;
  time(&now);
  printf("in : %s", ctime(&now));

//--------------------------------------//
  char url[] = "https://dts.bustecz.com/dts_api/getsch.php";
  char postData[] = "date=2022-05-06&station=10001&route=18";

  CURL *curl;
  CURLcode res;

  struct MemoryStruct chunk;
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
    if (res != CURLE_OK){
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }else{
      json_object *root = json_tokener_parse(chunk.memory);
      const char *str;
      int i;
      int n = json_object_array_length(root);

      // Remove all widget in container named vbox_c. //
      gtk_container_foreach(GTK_CONTAINER(widget), (void*) gtk_widget_destroy, NULL);

      GtkWidget *lbl=NULL;
      GtkWidget *hbox_c;
      for (i = 0; i<n; i++){
        
        str = json_object_get_string(json_object_array_get_idx(root, i));
        json_object *sch = json_tokener_parse(str);
        json_object *objTime = json_object_object_get(sch, "dep_time");
        json_object *objDest = json_object_object_get(sch, "dep_dest");
        json_object *objBusno = json_object_object_get(sch, "dep_busno");
        json_object *objStandard = json_object_object_get(sch, "dep_standard");
        json_object *objPlatform = json_object_object_get(sch, "dep_platform");

        //gchar *depDepart   = json_object_get_string(json_object_object_get(sch, "dep_depart"));

        hbox_c = gtk_hbox_new(FALSE, 5);
        gtk_box_pack_start(GTK_BOX(widget), hbox_c, FALSE, FALSE, 0);
        set_label(hbox_c, lbl, 5 , json_object_get_string(objTime), FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);
        set_label(hbox_c, lbl, 10, json_object_get_string(objDest), FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);
        set_label(hbox_c, lbl, 7 , json_object_get_string(objBusno), FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);
        set_label(hbox_c, lbl, 8 , json_object_get_string(objStandard), FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);
        set_label(hbox_c, lbl, 7 , json_object_get_string(objPlatform), FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);
        //set_label(hbox_c, lbl, 15, depStatus, FONT_SIZE, (*row[5] == '1')?CONTENT_COLOR:"#ff0000", FALSE, FALSE);
        //g_printf("%s\t%s\t%s\t%s\n", depTime, depDest, depBusno, depStandard);
        
        json_object_put(sch);
        
      }

      gtk_widget_show_all(win);
      //g_print("Server: %s,\n%s\n", SERVER, sql_buf);

      json_object_put(root);
    }
  }

  curl_easy_cleanup(curl);
  free(chunk.memory);
  curl_global_cleanup();

  //time_t now;
  time(&now);
  printf("out: %s", ctime(&now));

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

  //if (remove("dts.log") == 0)
  //  g_print("Delete file dts.log successfully.\n");
  //fp= fopen("dts.log", "a+");
  //fprintf (fp, "Current dir: %s\n", home);

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

  //fprintf(fp, "Normal ending program.\n");
  //fclose(fp);

  return 0;
}
