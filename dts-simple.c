#include <gtk/gtk.h>
#include <glib/gprintf.h>

GtkWidget *hbox1;

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
  { "17:00", "เชียงใหม่", "18-102", "ม.1ข", "1", "", FALSE},
  { "17:30", "เชียงของ", "962-1", "ม.4ข", "3", "", FALSE},
  { "18:00", "แม่สาย", "957-3", "ม.1ก", "ประตู 3", "รถติดขาเข้า", FALSE},
  { "18:05", "เชียงราย", "909-12", "ม.4ข", "4", "", FALSE},
  { "18:30", "แม่ฮ่องสอน", "961-1", "ม.1พ", "5", "", FALSE},
  { "19:00", "น่าน", "910-3", "ม.1ข", "1", "", FALSE},
  { "19:15", "ทุ่งช้าง", "47-7", "ม.1ก", "ประตู 3", "", FALSE},
  { "19:30", "เชียงราย", "909-1", "ม.1ก", "ประตู 3", "", FALSE},
  { "19:45", "เชียงใหม่", "18-52", "ม.1ข", "4", "", FALSE},
};


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

  gdk_color_parse(color, &lblColor);
  gdk_color_parse("#101010", &lblBgColor);
  
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

  image = load_pixbuf_from_file ("1397755.jpg");
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
 
  dfDateTime = pango_font_description_from_string("Laksaman");
  pango_font_description_set_size(dfDateTime, 20*PANGO_SCALE);
  
  dfName = pango_font_description_from_string("Laksaman");
  pango_font_description_set_size(dfName, 26*PANGO_SCALE);
  
  pango_font_description_set_weight(dfName, PANGO_WEIGHT_BOLD);
  pango_font_description_set_weight(dfDateTime, PANGO_WEIGHT_BOLD);

  gdk_color_parse("#006000", &NameColor);
  gtk_widget_modify_fg(GTK_WIDGET(lblDateTime), GTK_STATE_NORMAL, &NameColor);
  gtk_widget_modify_fg(GTK_WIDGET(lblName), GTK_STATE_NORMAL, &NameColor);

  gtk_widget_modify_font(lblDateTime, dfDateTime);
  gtk_widget_modify_font(lblName, dfName);
  gtk_box_pack_start(GTK_BOX(hbox), lblName, FALSE, FALSE, 10);
  gtk_box_pack_end(GTK_BOX(hbox), lblDateTime, FALSE, FALSE, 20);
  
  //header
  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE,0);
  set_label(hbox, lblHeader, 5, "เวลา", 30, "#006000", TRUE, TRUE);
  set_label(hbox, lblHeader, 10, "ปลายทาง", 30, "#006000", TRUE, TRUE);
  set_label(hbox, lblHeader, 7, "เลขรถ", 30, "#006000", TRUE, TRUE);
  set_label(hbox, lblHeader, 8, "มาตรฐาน", 30, "#006000", TRUE, TRUE);
  set_label(hbox, lblHeader, 7, "ชานชาลา", 30, "#006000", TRUE, TRUE);
  set_label(hbox, lblHeader, 15, "หมายเหตุ", 30, "#006000", TRUE, TRUE);
  //

  int i;
  /* add data to the list store */
  for (i = 0; i < G_N_ELEMENTS (data); i++){
    GtkWidget *lbl=NULL;
    hbox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
    set_label(hbox, lbl, 5, data[i].time, 30, "#00ff00", FALSE, FALSE); 
    set_label(hbox, lbl, 10, data[i].dest, 30, "#00ff00", FALSE, FALSE); 
    set_label(hbox, lbl, 7, data[i].busno, 30, "#00ff00", FALSE, FALSE); 
    set_label(hbox, lbl, 8, data[i].standard, 30, "#00ff00", FALSE, FALSE);    
    set_label(hbox, lbl, 7, data[i].platform, 30, "#00ff00", FALSE, FALSE);     
    set_label(hbox, lbl, 15, data[i].note, 30, "#00ff00", FALSE, FALSE);     
  }

  g_timeout_add (1000, (GSourceFunc)update_time, lblDateTime);
  g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(win);
  gtk_main();
  
  return 0;
}
