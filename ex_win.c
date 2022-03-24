#include <gtk/gtk.h>

static GtkWidget *window = NULL;
static GtkTreeModel *model = NULL;

static GtkTreeModel *create_and_fill_model (void);
static GtkWidget *create_view_and_model(void);

typedef struct
{
  const gchar    *time;
  const gchar    *dest;
  const gchar    *busno;
  const gchar    *standard;
  const gchar    *platform;
  const gchar    *note;
  const gboolean out;
  const gchar    *objname;
}
Table;

enum
{
  COL_TIME,
  COL_DEST,
  COL_BUSNO,
  COL_STANDARD,
  COL_PLATFORM,
  COL_NOTE,
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
  {"19:15", "เชียงใหม่", "18-1", "ม.1ก", "ประตู 3", "รถมาจากหมอชิต, รถติดหน้าหมอชิต", FALSE},
  {"19:30", "เชียงราย", "909-1", "ม.1ก", "ประตู 3", "", FALSE},
  {"19:45", "น่าน", "910-2", "ม.1ข", "4", "", FALSE},
};

static GtkWidget *create_view_and_model(void){
  GtkCellRenderer *renderer;
  //GtkTreeModel *model;
  GtkWidget *view;
  //GtkTreeViewColumn   *col;
  
  view = gtk_tree_view_new();
 
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "เวลา", renderer, "text", COL_TIME, NULL);
  
  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "ปลายทาง", renderer, "text", COL_DEST, NULL);

  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "เลขรถ", renderer, "text", COL_BUSNO, NULL);

  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "มาตรฐาน", renderer, "text", COL_STANDARD, NULL);

  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "ชานชาลา", renderer, "text", COL_PLATFORM, NULL);

  renderer = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "หมายเหตุ", renderer, "text", COL_NOTE, NULL);

  GtkTreeViewColumn *col;
  col = gtk_tree_view_get_column(GTK_TREE_VIEW(view), 5);
  gtk_tree_view_column_set_sizing(col, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_max_width(col, 100);
  gtk_tree_view_column_set_fixed_width(col, 100);
  
  model = create_and_fill_model();
  gtk_tree_view_set_search_column(GTK_TREE_VIEW(view), COL_PLATFORM);
  gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);

  
  g_object_unref(model);
  return view;
}

static GtkTreeModel *create_and_fill_model(void){
  GtkListStore *store;
  GtkTreeIter iter;
  
  store = gtk_list_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
  
  /*Append a row and fill in some data */
  
  int i;
  for (i = 0; i < G_N_ELEMENTS (data); i++){
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(
      store, &iter, COL_TIME, data[i].time, COL_DEST, data[i].dest, COL_BUSNO, data[i].busno, COL_STANDARD, data[i].standard, 
      COL_PLATFORM, data[i].platform, COL_NOTE, data[i].note, -1);
  }
  
  return GTK_TREE_MODEL(store);
}

GtkWidget *
do_list_store (GtkWidget *do_widget)
{
  if (!window){
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *hboxl;
    GtkWidget *hboxr;
    GtkWidget *vboxl;
    GtkWidget *vboxr;
    GtkWidget *label;
    GtkWidget *view;

    
    // create window, etc
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //gtk_window_set_screen (GTK_WINDOW (window),
    //                       gtk_widget_get_screen (do_widget));
    gtk_window_set_title (GTK_WINDOW (window), "ตารางเวลารถออก");
    g_signal_connect (window, "destroy",
                     G_CALLBACK (gtk_widget_destroyed), &window);
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);


    // Create layouts.
    vbox = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
        
    hbox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    
    hboxl = gtk_hbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(hbox), hboxl);  // important
    
    vboxl = gtk_vbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(hboxl), vboxl);
    
    label = gtk_label_new("ตารางรถออก");
    gtk_misc_set_alignment (GTK_MISC (label), 0, 0);
    gtk_box_pack_start(GTK_BOX(vboxl), label, FALSE, FALSE, 5);
        
    hboxr = gtk_hbox_new(FALSE, 5);
    gtk_container_add(GTK_CONTAINER(hbox), hboxr);  // important
    
    label = gtk_label_new(":::");
    gtk_box_pack_start(GTK_BOX(hboxr), label, FALSE, FALSE, 5);
    (void)vboxr;
    //
    
    // Add GtkTreeView to hboxl
    view = create_view_and_model();
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(view), TRUE);
    gtk_box_pack_start(GTK_BOX(vboxl), view, FALSE, FALSE, 0);    
    //

    if (!gtk_widget_get_visible (window)){
      gtk_widget_show_all (window);
    }else{
      gtk_widget_destroy (window);
      window = NULL;
    }
  }
  return window;
}

int main(int argc, char *argv[]){
  gtk_init(&argc, &argv);

  window = do_list_store(NULL);

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_widget_show_all(window);
  gtk_main();
  
  return 0;
}
