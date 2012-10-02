#include <gtk/gtk.h>

enum
  {
    COL_NAME = 0,
    COL_AGE,
    NUM_COLS
  } ;


static GtkTreeModel *
create_and_fill_model (void)
{
  GtkListStore  *store;
  GtkTreeIter    iter;

  store = gtk_list_store_new (NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);

  /* Append a row and fill in some data */
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter,
                      COL_NAME, "Heinz El-Mann",
                      COL_AGE, 51,
                      -1);

  /* append another row and fill in some data */
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter,
                      COL_NAME, "Jane Doe",
                      COL_AGE, 23,
                      -1);

  /* ... and a third row */
  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter,
                      COL_NAME, "Joe Bungop",
                      COL_AGE, 91,
                      -1);

  return GTK_TREE_MODEL (store);
}

static GtkWidget *
create_view_and_model (void)
{
  GtkCellRenderer     *renderer;
  GtkTreeModel        *model;
  GtkWidget           *view;

  view = gtk_tree_view_new ();

  /* --- Column #1 --- */

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,
                                               "Name",
                                               renderer,
                                               "text", COL_NAME,
                                               NULL);

  /* --- Column #2 --- */

  renderer = gtk_cell_renderer_text_new ();
  gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view),
                                               -1,
                                               "Age",
                                               renderer,
                                               "text", COL_AGE,
                                               NULL);

  model = create_and_fill_model ();

  gtk_tree_view_set_model (GTK_TREE_VIEW (view), model);

  /* The tree view has acquired its own reference to the
   *  model, so we can drop ours. That way the model will
   *  be freed automatically when the tree view is destroyed */

  g_object_unref (model);

  return view;
}

static GtkWidget *
create_scrolled_window (void)
{
  GtkWidget *scrolled_window;

  /* 新しくスクロールドウィンドウを作成 */
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);

  gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);

  /* ポリシーは GTK_POLICY AUTOMATIC か GTK_POLICY_ALWAYS のどちらかである。
   * GTK_POLICY_AUTOMATIC は自動的にスクロールバーが必要か判断するのに対し、
   * GTK_POLICY_ALWAYS は常にスクロールバーを表示する。
   * 最初に設定しているのが水平方向で、次が垂直方向である。*/

  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  return scrolled_window;
}

int
main (int argc, char **argv)
{
  GtkWidget *window;
  GtkWidget *scrolled_window;
  GtkWidget *view;

  gtk_init (&argc, &argv);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "delete_event", gtk_main_quit, NULL); /* dirty */

  scrolled_window = create_scrolled_window();
  gtk_container_add (GTK_CONTAINER (window), scrolled_window);
  /* this is done with gtk_widget_show_all? */
  /* gtk_widget_show (scrolled_window); */

  view = create_view_and_model ();

  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window),
                                         view);

  gtk_widget_show_all (window);

  gtk_main ();

  return 0;
}
