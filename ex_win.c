#include <gtk/gtk.h>

static GtkWidget *window = NULL;
static GtkTreeModel *model = NULL;

typedef struct
{
  const gchar 	*time;
  const gchar		*dest;
  const gchar		*busno;
  const gchar		*standard;
  const gchar		*platform;
  const gchar		*note;
  const gboolean	out;
  const gchar		*objname;
}
Table;

enum
{
  COLUMN_TIME,
  COLUMN_DEST,
  COLUMN_BUSNO,
  COLUMN_STANDARD,
  COLUMN_PLATFORM,
  COLUMN_NOTE,
  NUM_COLUMNS
};

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
window_closed (GtkWidget *widget,
               GdkEvent  *event,
               gpointer   user_data)
{
  model = NULL;
  window = NULL;
  return FALSE;
}

static GtkTreeModel *
create_model (void)
{
  gint i = 0;
  GtkListStore *store;
  GtkTreeIter iter;

  /* create list store */
  store = gtk_list_store_new (NUM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  /* add data to the list store */
  for (i = 0; i < G_N_ELEMENTS (data); i++)
    {
      gtk_list_store_append (store, &iter);
      gtk_list_store_set (store, &iter,
                          COLUMN_TIME, data[i].time,
                          COLUMN_DEST, data[i].dest,
                          COLUMN_BUSNO, data[i].busno,
                          COLUMN_STANDARD, data[i].standard,
                          COLUMN_PLATFORM, data[i].platform,
                          COLUMN_NOTE, "", data[i].note
                          -1);
    }

  return GTK_TREE_MODEL (store);
}

static void
add_columns (GtkTreeView *treeview)
{
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  //GtkTreeModel *model = gtk_tree_view_get_model (treeview);

  /* column for bug numbers */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("เวลา",
                                                     renderer,
                                                     "text",
                                                     COLUMN_TIME,
                                                     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COLUMN_TIME);
  gtk_tree_view_append_column (treeview, column);

  /* column for severities */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("ปลายทาง",
                                                     renderer,
                                                     "text",
                                                     COLUMN_DEST,
                                                     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COLUMN_DEST);
  gtk_tree_view_append_column (treeview, column);

  /* column for description */
  renderer = gtk_cell_renderer_text_new ();
  column = gtk_tree_view_column_new_with_attributes ("หมายเลขรถ",
                                                     renderer,
                                                     "text",
                                                     COLUMN_BUSNO,
                                                     NULL);
  gtk_tree_view_column_set_sort_column_id (column, COLUMN_BUSNO);
  gtk_tree_view_append_column (treeview, column);

  /* มาตรฐาน */
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("มาตรฐาน", renderer, "text", COLUMN_STANDARD, NULL);
  gtk_tree_view_column_set_sort_column_id(column, COLUMN_STANDARD);
  gtk_tree_view_append_column(treeview, column);
  
  /* ชานชาลา */
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("ชานชาลา", renderer, "text", COLUMN_PLATFORM, NULL);
  gtk_tree_view_column_set_sort_column_id(column, COLUMN_PLATFORM);
  gtk_tree_view_append_column(treeview, column);
  
  /* หมายเหตุ */
  renderer = gtk_cell_renderer_text_new();
  column = gtk_tree_view_column_new_with_attributes("หมายเหตุ", renderer, "text", COLUMN_NOTE, NULL);
  gtk_tree_view_column_set_sort_column_id(column, COLUMN_NOTE);
  gtk_tree_view_append_column(treeview, column);
}
//COLUMN_TIME, COLUMN_DEST, COLUMN_BUSNO, COLUMN_STANDARD, COLUMN_PLATFORM, COLUMN_NOTE, NUM_COLUMNS

GtkWidget *
do_list_store (GtkWidget *do_widget)
{
  if (!window)
    {
      GtkWidget *vbox;
      GtkWidget *label;
      GtkWidget *sw;
      GtkWidget *treeview;
      
      GtkWidget *hbox;
      GtkWidget *hboxl;
      GtkWidget *hboxr;

      // create window, etc
      window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
      gtk_window_set_screen (GTK_WINDOW (window),
                             gtk_widget_get_screen (do_widget));
      gtk_window_set_title (GTK_WINDOW (window), "ตารางเวลารถออก");

      g_signal_connect (window, "destroy",
                        G_CALLBACK (gtk_widget_destroyed), &window);
      //gtk_container_set_border_width (GTK_CONTAINER (window), 8);

      hbox = gtk_hbox_new(FALSE, 5);
      gtk_container_add(GTK_CONTAINER(window), hbox);
  
      hboxl = gtk_hbox_new(FALSE, 5);
      gtk_container_add(GTK_CONTAINER(hbox), hboxl);
      //gtk_box_pack_start(GTK_BOX(hbox), hboxl, FALSE, FALSE, 5);
      
      hboxr = gtk_hbox_new(FALSE, 5);
      gtk_container_add(GTK_CONTAINER(hbox), hboxr);
      label = gtk_label_new(":::");
      gtk_box_pack_start(GTK_BOX(hboxr), label, FALSE, FALSE, 5);
      
      vbox = gtk_vbox_new(FALSE, 10);
      gtk_container_add(GTK_CONTAINER(hboxl), vbox);
      
      label = gtk_label_new("ตารางรถออก");
      gtk_container_add(GTK_CONTAINER(vbox), label);
      
      sw = gtk_scrolled_window_new (NULL, NULL);
      gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),
                                           GTK_SHADOW_ETCHED_IN);
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                      GTK_POLICY_NEVER,
                                      GTK_POLICY_AUTOMATIC);
      
      gtk_box_pack_start (GTK_BOX (vbox), sw, TRUE, TRUE, 0);

      // create tree model 
      model = create_model ();

      // create tree view
      treeview = gtk_tree_view_new_with_model (model);
      gtk_tree_view_set_rules_hint (GTK_TREE_VIEW (treeview), TRUE);
      //gtk_tree_view_set_search_column (GTK_TREE_VIEW (treeview), COLUMN_DESCRIPTION);

      g_object_unref (model);

      gtk_container_add (GTK_CONTAINER (sw), treeview);

      // add columns to the tree view 
      add_columns (GTK_TREE_VIEW (treeview));

      // finish & show 
      gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
      g_signal_connect (window, "delete-event",
                        G_CALLBACK (window_closed), NULL);
    }

  if (!gtk_widget_get_visible (window))
    {
      gtk_widget_show_all (window);
    }
  else
    {
      gtk_widget_destroy (window);
      window = NULL;
    }

  return window;
}

int main(int argc, char *argv[]){
  GtkWidget *window;
/*
  GtkWidget *vbox;
  GtkWidget *hbox;
  GtkWidget *hboxl;
  GtkWidget *hboxr;
  GtkWidget *label;
  //GtkWidget *button;
*/
  gtk_init(&argc, &argv);

  window = do_list_store(NULL);

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_widget_show_all(window);
  gtk_main();
  
  return 0;
}
