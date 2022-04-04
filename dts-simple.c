#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include "dts_functions.h"

typedef struct
{
  const gchar 		*time;
  const gchar		*dest;
  const gchar		*busno;
  const gchar		*standard;
  const gchar		*platform;
  const gchar		*note;
  const gboolean	out;
  const gchar		*objname;
}
Table;

static Table data[] =
{
  {"17:00", "เชียงใหม่", "18-102", "ม.1ข", "1", "", FALSE},
  {"17:30", "เชียงของ", "962-1", "ม.4ข", "3", "", FALSE},
  {"18:00", "แม่สาย", "957-3", "ม.1ก", "ประตู 3", "", FALSE},
  {"18:05", "เชียงราย", "909-12", "ม.4ข", "4", "", FALSE},
  {"18:30", "แม่ฮ่องสอน", "961-1", "ม.1พ", "5", "", FALSE},
  {"19:00", "เชียงใหม่", "18-1", "ม.1ข", "1", "", FALSE},
  {"19:15", "เชียงใหม่", "18-1", "ม.1ก", "ประตู 3", "", FALSE},
  {"19:30", "เชียงราย", "909-1", "ม.1ก", "ประตู 3", "", FALSE},
  {"19:45", "น่าน", "910-2", "ม.1ข", "4", "", FALSE},
};

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

int main(int argc, char *argv[]){

  GtkWidget *win=NULL;
  GdkPixbuf *icon;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *hbox_c[9];
  GtkWidget *lblDateTime;
  GtkWidget *lblName = NULL;
  GtkWidget *lblHeader = NULL;
  PangoFontDescription *dfDateTime;
  PangoFontDescription *dfName;
  
  GdkPixbuf *image = NULL;
  GdkPixmap *background = NULL;
  GtkStyle *style = NULL;
  GdkColor NameColor;

  gtk_init(&argc, &argv);
  
  // Read file dts.conf to setup color, font and font size.
  gchar *BG_IMAGE = config_get_string("dts.conf", "Images", "BG_IMAGE");
  gchar *TITLE_TEXT = config_get_string("dts.conf", "Title", "TITLE_TEXT");
  gchar *TITLE_FONT = config_get_string("dts.conf", "Title", "TITLE_FONT");
  gint TITLE_SIZE = config_get_integer("dts.conf", "Title", "TITLE_SIZE");
  gchar *HEADER_COLOR = config_get_string("dts.conf", "Color", "HEADER_COLOR");
  gchar *CONTENT_COLOR = config_get_string("dts.conf", "Color", "CONTENT_COLOR");
  gchar *FONT_SIZE_F = config_get_string("dts.conf", "Contents", "FONT_SIZE");
  gint FONT_SIZE = config_get_integer("dts.conf", "Contents", FONT_SIZE_F);
  g_free(FONT_SIZE_F);

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
  //pango_font_description_set_weight(dfDateTime, PANGO_WEIGHT_BOLD);

  gdk_color_parse(HEADER_COLOR, &NameColor);
  gtk_widget_modify_fg(GTK_WIDGET(lblDateTime), GTK_STATE_NORMAL, &NameColor);
  gtk_widget_modify_fg(GTK_WIDGET(lblName), GTK_STATE_NORMAL, &NameColor);

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
  set_label(hbox, lblHeader, 15, "หมายเหตุ", FONT_SIZE, HEADER_COLOR, TRUE, TRUE);
  //

  int i;
  /* add data to the list store */
  //for (i = 0; i < G_N_ELEMENTS (data); i++){
  for (i = 0; i < 9; ++i){
    GtkWidget *lbl=NULL;
    
    hbox_c[i] = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_c[i], FALSE, FALSE, 0);
    set_label(hbox_c[i], lbl, 5, data[i].time, FONT_SIZE, CONTENT_COLOR, FALSE, FALSE); 
    set_label(hbox_c[i], lbl, 10, data[i].dest, FONT_SIZE, CONTENT_COLOR, FALSE, FALSE); 
    set_label(hbox_c[i], lbl, 7, data[i].busno, FONT_SIZE, CONTENT_COLOR, FALSE, FALSE); 
    set_label(hbox_c[i], lbl, 8, data[i].standard, FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);    
    set_label(hbox_c[i], lbl, 7, data[i].platform, FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);     
    set_label(hbox_c[i], lbl, 15, data[i].note, FONT_SIZE, CONTENT_COLOR, FALSE, FALSE);     
  }

  g_timeout_add (1000, (GSourceFunc)update_time, lblDateTime);
  g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(win);
  gtk_main();
  
  return 0;
}
