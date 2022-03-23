extern gchar *config_get_string(gchar *file, gchar *group, gchar *key);
extern gint config_get_integer(gchar *file, gchar *group, gchar *key);
extern GdkPixbuf *load_pixbuf_from_file (const char *filename);
extern GdkPixbuf *create_pixbuf(const gchar *filename);
extern void set_label(GtkWidget *hbox, GtkWidget *lbl, gint n_char, const gchar *text, gint size, gchar *color, gboolean BG_SET, gboolean BOLD);
