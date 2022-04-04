#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include "dts_functions.h"

void
set_label(GtkWidget *hbox, GtkWidget *lbl, gint n_char, const gchar *text, gint size, gchar *color, gboolean BG_SET, gboolean BOLD)
{
  PangoFontDescription *dfLabel;
  GdkColor lblColor;
  GdkColor lblBgColor;
  
  gchar *BG_COLOR = config_get_string("dts.conf", "Color", "BG_COLOR");
  gchar *CONTENT_FONT = config_get_string("dts.conf", "Contents", "CONTENT_FONT");

  gdk_color_parse(color, &lblColor);
  gdk_color_parse(BG_COLOR, &lblBgColor);

  dfLabel = pango_font_description_from_string(CONTENT_FONT);
  //if (BOLD)
    //pango_font_description_set_weight(dfLabel, PANGO_WEIGHT_BOLD);

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

gchar 
*config_get_string(gchar *file, gchar *group, gchar *key)
{
  GKeyFile *key_file;
  GError *error;
  //gsize length;
  gchar *val;
  
  gchar *home = g_get_home_dir();
  g_print("Home: %s\n", home);

  g_chdir("/home/yothinin/projects/dts");

  key_file = g_key_file_new();
  error = NULL;
    
  if(!g_key_file_load_from_file(key_file, file, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error)){
    g_debug("%s", error->message);
  }else{
    val = g_key_file_get_string(key_file, group, key, &error);    
  }

  return val;
}

gint
config_get_integer(gchar *file, gchar *group, gchar *key)
{
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

GdkPixbuf
*load_pixbuf_from_file (const char *filename)
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
