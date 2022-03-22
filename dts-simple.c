#include <gtk/gtk.h>
#include <glib/gprintf.h>

GtkWidget *hbox1;

typedef struct
{
  const gchar       *id;
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
  {"1", "17:00", "เชียงใหม่", "18-102", "ม.1ข", "1", "", FALSE},
  {"2", "17:30", "เชียงของ", "962-1", "ม.4ข", "3", "", FALSE},
  {"3", "18:00", "แม่สาย", "957-3", "ม.1ก", "ประตู 3", "รถติดหน้าหมอชิต", FALSE},
  {"4", "18:05", "เชียงราย", "909-12", "ม.4ข", "4", "", FALSE},
  {"5", "18:30", "แม่ฮ่องสอน", "961-1", "ม.1พ", "5", "", FALSE},
  {"6", "19:00", "เชียงใหม่", "18-1", "ม.1ข", "1", "", FALSE},
  {"7", "19:15", "เชียงใหม่", "18-1", "ม.1ก", "ประตู 3", "", FALSE},
  {"8", "19:30", "เชียงราย", "909-1", "ม.1ก", "ประตู 3", "", FALSE},
  {"9", "19:45", "น่าน", "910-2", "ม.1ข", "4", "", FALSE},
};

static gchar *config_get_string(gchar *file, gchar *group, gchar *key){
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

static gint config_get_integer(gchar *file, gchar *group, gchar *key){
  GKeyFile *key_file;
  GError *error;
  gint val;
  
  key_file = g_key_file_new();
  error = NULL;
  if (!g_key_file_load_from_file(key_file, file, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error)){
    g_debug("%s", error->message);
  }else{
    val = g_key_file_get_integer(key_file, group, key, &error);
  }
  
  return val;
}  

GdkPixbuf *load_pixbuf_from_file (const char *filename)
{
    GError *error = NULL;
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (filename, &error);

    if (pixbuf == NULL)
    {
        g_print ("Error loading file: %d : %s\n", error->code, error->message);
        g_error_free (error);
        exit (1);
    }
    return pixbuf;
}

gchar buf[256];

GdkPixbuf *create_pixbuf(const gchar *filename){
  GdkPixbuf *pixbuf;
  GError *error = NULL;
  pixbuf = gdk_pixbuf_new_from_file(filename, &error);
  
  if (!pixbuf){
    fprintf(stderr, "%s\n", error->message);
    g_error_free(error);
  }
  return pixbuf;
}

static gboolean
update_time(GtkWidget *label)
{
  GDateTime *now = g_date_time_new_now_local();
  gchar *mytime = g_date_time_format(now, "%d-%m-%y, %H:%M:%S");
  g_sprintf(buf, "%s", mytime);
  g_free(mytime);
  g_date_time_unref(now);
  
  gtk_label_set_text(GTK_LABEL(label), buf);
  
  return TRUE;
}

void
set_label(GtkWidget *hbox, GtkWidget *lbl, gint n_char, const gchar *text, gint size, gchar *color, gboolean BG_SET, gboolean BOLD)
{
  PangoFontDescription *dfLabel;
  GdkColor lblColor;
  GdkColor lblBgColor;
  
  gchar *BG_COLOR = config_get_string("dts.conf", "Color", "BG_COLOR");

  gdk_color_parse(color, &lblColor);
  gdk_color_parse(BG_COLOR, &lblBgColor);
  
  if (BOLD)
    dfLabel = pango_font_description_from_string("Norasi Bold");
  else
    dfLabel = pango_font_description_from_string("Norasi Regular");

  pango_font_description_set_size(dfLabel, size*PANGO_SCALE);

  lbl = gtk_label_new(text);
  gtk_widget_modify_fg(GTK_WIDGET(lbl), GTK_STATE_NORMAL, &lblColor);
  gtk_widget_modify_font(lbl, dfLabel);
  
  gtk_label_set_width_chars(GTK_LABEL(lbl), n_char);

  GtkWidget *eventbox;
  eventbox = gtk_event_box_new();
  if (BG_SET){
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &lblBgColor);
  }else
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(eventbox), FALSE);

  gtk_misc_set_alignment(GTK_MISC(lbl), 0, 0.5);
  gtk_misc_set_padding(GTK_MISC(lbl), 15, 0);
  
  gtk_container_add(GTK_CONTAINER(eventbox), lbl);
  gtk_box_pack_start(GTK_BOX(hbox), eventbox, FALSE, FALSE, 10);
}

void remove_first(GtkWidget *button, gpointer user_data){
  gtk_container_remove(GTK_CONTAINER(user_data), hbox1);
}

int main(int argc, char *argv[]){

  GtkWidget *win=NULL;
  GdkPixbuf *icon;
  GtkWidget *vbox;
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

  gtk_init(&argc, &argv);
  
  gchar *BG_IMAGE = config_get_string("dts.conf", "Images", "BG_IMAGE");

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
  
  lblName = gtk_label_new("::ตารางเวลารถออก");
  lblDateTime = gtk_label_new("");
 
  gchar *TITLE_FONT = config_get_string("dts.conf", "Fonts", "TITLE_FONT");
  gint TITLE_SIZE = config_get_integer("dts.conf", "Fonts", "TITLE_SIZE");
 
  dfDateTime = pango_font_description_from_string(TITLE_FONT);
  pango_font_description_set_size(dfDateTime, TITLE_SIZE*PANGO_SCALE);
  
  dfName = pango_font_description_from_string(TITLE_FONT);
  pango_font_description_set_size(dfName, TITLE_SIZE*PANGO_SCALE);
  
  pango_font_description_set_weight(dfName, PANGO_WEIGHT_BOLD);
  pango_font_description_set_weight(dfDateTime, PANGO_WEIGHT_BOLD);

  gchar *HEADER_COLOR = config_get_string("dts.conf", "Color", "HEADER_COLOR");

  gdk_color_parse(HEADER_COLOR, &NameColor);
  gtk_widget_modify_fg(GTK_WIDGET(lblDateTime), GTK_STATE_NORMAL, &NameColor);
  gtk_widget_modify_fg(GTK_WIDGET(lblName), GTK_STATE_NORMAL, &NameColor);

  gtk_widget_modify_font(lblDateTime, dfDateTime);
  gtk_widget_modify_font(lblName, dfName);
  gtk_box_pack_start(GTK_BOX(hbox), lblName, FALSE, FALSE, 10);
  gtk_box_pack_end(GTK_BOX(hbox), lblDateTime, FALSE, FALSE, 20);
  
  gchar *CHAR_WIDTH_F = config_get_string("dts.conf", "Width", "CHAR_WIDTH");
  gint CHAR_WIDTH = config_get_integer("dts.conf", "Width", CHAR_WIDTH_F);
  g_free(CHAR_WIDTH_F);
  
  //header
  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE,0);
  set_label(hbox, lblHeader, 5, "เวลา", CHAR_WIDTH, HEADER_COLOR, TRUE, TRUE);
  set_label(hbox, lblHeader, 10, "ปลายทาง", CHAR_WIDTH, HEADER_COLOR, TRUE, TRUE);
  set_label(hbox, lblHeader, 7, "เลขรถ", CHAR_WIDTH, HEADER_COLOR, TRUE, TRUE);
  set_label(hbox, lblHeader, 8, "มาตรฐาน", CHAR_WIDTH, HEADER_COLOR, TRUE, TRUE);
  set_label(hbox, lblHeader, 7, "ชานชาลา", CHAR_WIDTH, HEADER_COLOR, TRUE, TRUE);
  set_label(hbox, lblHeader, 15, "หมายเหตุ", CHAR_WIDTH, HEADER_COLOR, TRUE, TRUE);
  //

  gchar *CONTENT_COLOR = config_get_string("dts.conf", "Color", "CONTENT_COLOR");

  int i;
  /* add data to the list store */
  for (i = 0; i < G_N_ELEMENTS (data); i++){
    GtkWidget *lbl=NULL;
    
    gchar hb[10];
    sprintf(hb, "%s%s", "hbox", "1");
    g_print("%s\n", hb);
    
    hbox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    set_label(hbox, lbl, 5, data[i].time, CHAR_WIDTH, CONTENT_COLOR, FALSE, FALSE); 
    set_label(hbox, lbl, 10, data[i].dest, CHAR_WIDTH, CONTENT_COLOR, FALSE, FALSE); 
    set_label(hbox, lbl, 7, data[i].busno, CHAR_WIDTH, CONTENT_COLOR, FALSE, FALSE); 
    set_label(hbox, lbl, 8, data[i].standard, CHAR_WIDTH, CONTENT_COLOR, FALSE, FALSE);    
    set_label(hbox, lbl, 7, data[i].platform, CHAR_WIDTH, CONTENT_COLOR, FALSE, FALSE);     
    set_label(hbox, lbl, 15, data[i].note, CHAR_WIDTH, CONTENT_COLOR, FALSE, FALSE);     
  }

  g_timeout_add (1000, (GSourceFunc)update_time, lblDateTime);
  g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(win);
  gtk_main();
  
  return 0;
}
