#include <gtk/gtk.h>
#include <mysql.h>
#include <stdlib.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>

MYSQL *db_connect(const char *dbname);
void db_disconnect(MYSQL *db);
void db_do_query(MYSQL *db, const char *query);

static gboolean
update_label(GtkWidget *label)
{
  static int i = 0;
  MYSQL *db;
  g_print("DB connect...\n");
  db = db_connect("dts");
  gchar buf[256];
  g_sprintf(buf, "Connect to %s, (%d)\nStatus: %d\nRetry count: %d\nReconnect flag: %d\nLast error no: %d\nLast error: %s", db->db, ++i, db->status, db->net.retry_count, db->reconnect, db->net.last_errno, db->net.last_error);
  
  gtk_label_set_text(GTK_LABEL(label), buf);
  
  g_print("DB disconnect...\n");
  db_disconnect(db);
  
  return TRUE;
}

MYSQL *
db_connect(const char *dbname)
{
  MYSQL *db = mysql_init(NULL);
  if (!db)
    g_print("mysql_init() failed. (%s)\n", mysql_error(db));
  
  if (mysql_real_connect(db, "dts.bustecz.com", "orangepi_r", "0rangePi", dbname, 0, NULL, 0) == NULL)
    g_print("mysql_real_connect(), failed. \n(%d) %s\n", mysql_errno(db), mysql_error(db));
  
  return db;
}

void
db_disconnect(MYSQL *db)
{
  mysql_close(db);
}

int
main (int argc, char *argv[])
{
  GtkWidget *win;
  GtkWidget *vbox;
  GtkWidget *label;

  if (mysql_library_init(0, NULL, NULL)){
    g_print("Could not initiliz MySQL client library.\n");
    exit(1);
  }
  
  MYSQL *one, *two;
  
  one = db_connect("dts");
  two = db_connect(NULL);
  
  if (one == NULL)
    g_print("one, NULL.\n");
  
  if (two == NULL)
    g_print("two, NULL.\n");

  g_print("status: %d\n", one->reconnect);

  gtk_init(&argc, &argv);

  win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(win), 600, 400);
  gtk_window_set_title(GTK_WINDOW(win), "Test connect");
  
  vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_container_add(GTK_CONTAINER(win), vbox);
  
  label = gtk_label_new("Connect (0)");
  gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);
  
  g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_timeout_add (3000, (GSourceFunc)update_label, label);

  
  gtk_widget_show_all(win);
  
  gtk_main();

  db_disconnect(one);
  db_disconnect(two);

  mysql_library_end();
  
  return 0;
}
