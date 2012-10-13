#include<gtk/gtk.h>
#include<stdlib.h>
#include<string.h>

#include<assert.h>

#include<psqlite.h>

enum
  {
    COL_NAME = 0,
    COL_AGE,
    NUM_COLS
  } ;

void
list_store_set_value_from_data (GtkListStore* store, GtkTreeIter* iter_t, psqlite_data* data, int column)
{
  int n;
  double d;
  char* str;

  /* n = (int*) malloc(sizeof(int)); */

  int type = data->type;

  switch(type){
  case SQLITE_INTEGER:
    n = data->val.n;
    gtk_list_store_set(store, iter_t, column, n, -1);
    break;
  case SQLITE_FLOAT:
    d = data->val.d;
    gtk_list_store_set(store, iter_t, column, d, -1);
    break;
  case SQLITE_TEXT:
    str = data->val.s;
    value = (char *) str;
    gtk_list_store_set(store, iter_t, column, str, -1);
    break;
  case SQLITE_BLOB:
    str = "BLOB";
    gtk_list_store_set(store, iter_t, column, str, -1);
    break;
  case SQLITE_NULL:
    str = "NULL";
    break;
  default:
    printf("Unknown type.\n");
    str = "UNKNOWN";
    gtk_list_store_set(store, iter_t, column, str, -1);
    break;
  }
}

int
list_store_value_from_table (GtkListStore* store, GtkTreeIter* iter_t, psqlite_table* tb, int i)
{
  int j;

  for (j = 0; j < tb->column_num; j++){
    list_store_set_value_from_data(store, iter_t, &(tb->data[i][j]), j);
  }

  return rc;
}

GType*
get_column_types(psqlite_table* tb)
{
  int columns = tb->column_num;
  GType* a;
  int i;
  int type;

  a = (GType *) malloc(sizeof(GType) * columns);
  for(i = 0; i < columns; i++){
    /* all types are NULL untill sqlite3_step() is executed. */
    type = tb->data[0][i].type;

    switch(type){
    case SQLITE_INTEGER:
      a[i] = G_TYPE_INT;
      break;
    case SQLITE_FLOAT:
      a[i] = G_TYPE_FLOAT;
      break;
    case SQLITE_TEXT:
      a[i] = G_TYPE_STRING;
      break;
    case SQLITE_BLOB:
      a[i] = G_TYPE_STRING;
      break;
    case SQLITE_NULL:
      a[i] = G_TYPE_STRING;
      break;
    default:
      a[i] = G_TYPE_STRING;
      break;
    }
  }
  return a;
}

static GtkTreeModel *
create_and_fill_model (psqlite_table* tb)
{
  GtkListStore  *store;
  GtkTreeIter    iter;

  int columns;
  GType* types;

  int i;

  psqlite_table_fetch(tb, 0);

  columns = tb->column_num;
  types = get_column_types(tb);

  store = gtk_list_store_newv (columns, types);

  for(i = 0; i < tb->data_num, i++){
    gtk_list_store_append (store, &iter);
    list_store_value_from_data(store, &iter, tb, i);
  }

  return GTK_TREE_MODEL (store);
}

void
tree_view_insert_columns_from_tb(GtkWidget* view, psqlite_table* tb)
{
  int columns;
  int i;
  GtkCellRenderer* renderer;

  columns = tb->column_num;
  for(i = 0; i < columns; i++){
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                tb->columns[i],
                                                renderer,
                                                "text", i,
                                                NULL);
  }
}

static GtkWidget *
create_view_and_model (psqlite_table* tb)
{
  /* GtkCellRenderer     *renderer; */
  GtkTreeModel        *model;
  GtkWidget           *view;

  model = create_and_fill_model (tb);

  view = gtk_tree_view_new ();
  tree_view_insert_columns_from_table(view, tb);

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

void
create_cells_window (char* filename)
{
  GtkWidget *window;
  GtkWidget *scrolled_window;
  GtkWidget *view;

  psqlite* db;
  psqlite_table* tb;
  int rc;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "delete_event", gtk_main_quit, NULL); /* dirty */

  scrolled_window = create_scrolled_window();
  gtk_container_add (GTK_CONTAINER (window), scrolled_window);
  /* this is done with gtk_widget_show_all? */
  /* gtk_widget_show (scrolled_window); */

  psqlite_connect(filename, &db);
  /* assert(! rc); */

  int tn = 3;
  printf("%s\n", db->tables[tn]);
  psqlite_get_table(db, db->tables[tn], &tb);

  view = create_view_and_model(tb);

  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled_window),
                                         view);

  gtk_widget_show_all (window);

  return;
}

int
main (int argc, char **argv)
{
  gtk_init (&argc, &argv);
  assert(argc >= 2);
  create_cells_window (argv[1]);
  gtk_main();
  return 0;
}
