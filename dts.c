#include <cairo.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>

/*
struct schedule{
  char time[5];
  char bus_no[20];
  char dest[30];
}s[10];
*/

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
set_label(GtkWidget *hbox, GtkWidget *lbl, gint n_char, gchar *text, gint size, gchar *color, gboolean BG_SET)
{
  PangoFontDescription *dfLabel;
  GdkColor lblColor;
  GdkColor lblBgColor;

  gdk_color_parse(color, &lblColor);
  gdk_color_parse("black", &lblBgColor);
  
  dfLabel = pango_font_description_from_string("Sawasdee");
  pango_font_description_set_size(dfLabel, size*PANGO_SCALE);

  lbl = gtk_label_new(text);
  gtk_widget_modify_fg(GTK_WIDGET(lbl), GTK_STATE_NORMAL, &lblColor);
  gtk_widget_modify_font(lbl, dfLabel);
  
  gtk_label_set_width_chars(GTK_LABEL(lbl), n_char);

  GtkWidget *eventbox;
  eventbox = gtk_event_box_new();
  if (BG_SET)
    gtk_widget_modify_bg(eventbox, GTK_STATE_NORMAL, &lblBgColor);
  else
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(eventbox), FALSE);

  gtk_misc_set_alignment(GTK_MISC(lbl), 0, 0.5);
  gtk_misc_set_padding(GTK_MISC(lbl), 15, 0);
  
  gtk_container_add(GTK_CONTAINER(eventbox), lbl);
  gtk_box_pack_start(GTK_BOX(hbox), eventbox, FALSE, FALSE, 10);
}

int main(int argc, char *argv[]){

  GtkWidget *win=NULL;
  GdkPixbuf *icon;
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *lblDateTime;
  GtkWidget *lblName;
  GtkWidget *lblHeader = NULL;
  PangoFontDescription *dfDateTime;
  PangoFontDescription *dfName;
  
  GdkPixbuf *image = NULL;
  GdkPixmap *background = NULL;
  GtkStyle *style = NULL;
  
  GdkColor white;		/* color */

  gtk_init(&argc, &argv);

  image = load_pixbuf_from_file ("dts_bg.jpg");
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

  //GdkCursor *Cursor = gdk_cursor_new(GDK_BLANK_CURSOR);
  //GdkWindow *window = gtk_widget_get_window(win);
  //gdk_window_set_cursor(window, Cursor);

  icon = create_pixbuf("Digital-Signage.png");
  gtk_window_set_icon(GTK_WINDOW(win), icon);

  vbox = gtk_vbox_new(FALSE, 1);
  gtk_container_add(GTK_CONTAINER(win), vbox);
  
  hbox = gtk_hbox_new(FALSE, 1);
  //gtk_container_add(GTK_CONTAINER(vbox), hbox);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  
  lblName = gtk_label_new("ตารางเวลารถออก");
  lblDateTime = gtk_label_new("");
  
  dfDateTime = pango_font_description_from_string("Sawasdee");
  pango_font_description_set_size(dfDateTime, 20*PANGO_SCALE);
  
  dfName = pango_font_description_from_string("Sawasdee");
  pango_font_description_set_size(dfName, 40*PANGO_SCALE);

  gdk_color_parse("green", &white);
  gtk_widget_modify_fg(GTK_WIDGET(lblDateTime), GTK_STATE_NORMAL, &white);
  gtk_widget_modify_fg(GTK_WIDGET(lblName), GTK_STATE_NORMAL, &white);

  gtk_widget_modify_font(lblDateTime, dfDateTime);
  gtk_widget_modify_font(lblName, dfName);
  gtk_box_pack_start(GTK_BOX(hbox), lblName, FALSE, FALSE, 10);
  gtk_box_pack_end(GTK_BOX(hbox), lblDateTime, FALSE, FALSE, 20);
  
  //header
  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE,10);
  set_label(hbox, lblHeader, 5, "เวลา", 30, "light green", TRUE);
  set_label(hbox, lblHeader, 10, "ปลายทาง", 30, "light green", TRUE);
  set_label(hbox, lblHeader, 10, "เลขรถ", 30, "light green", TRUE);
  //
  
  //s[1].time = "17:00";

  GtkWidget *lbl=NULL;
  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  set_label(hbox, lbl, 5, "17:00", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "เชียงใหม่", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "18-102 (ม.1ข)", 30, "light green", FALSE); 

  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  set_label(hbox, lbl, 5, "18:00", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "957-1 (ม.1ก)", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "แม่สาย", 30, "light green", FALSE); 

  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  set_label(hbox, lbl, 5, "19:00", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "909-7 (ม.1พ)", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "เชียงราย", 30, "light green", FALSE); 

  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  set_label(hbox, lbl, 5, "19:30", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "961-2(ม.4ค)", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "แม่ฮ่องสอน", 30, "light green", FALSE); 

  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  set_label(hbox, lbl, 5, "19:45", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "962-5 (ม.1ข)", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "เชียงของ", 30, "light green", FALSE); 

  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  set_label(hbox, lbl, 5, "19:45", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "47-3 (ม.1ก)", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "ทุ่งช้าง", 30, "light green", FALSE); 

  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  set_label(hbox, lbl, 5,  "20:00", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "20-1 (ม.4ก)", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "ขอนแก่น", 30, "light green", FALSE); 

  hbox = gtk_hbox_new(FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  set_label(hbox, lbl, 5, "20:05", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "3-2 (ม.1ก)", 30, "light green", FALSE); 
  set_label(hbox, lbl, 10, "เขียงแสน", 30, "light green", FALSE); 

  
  g_timeout_add (1000, (GSourceFunc)update_time, lblDateTime);
  g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_widget_show_all(win);
  gtk_main();
  
  return 0;
}
