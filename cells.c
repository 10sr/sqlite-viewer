#include <gtk/gtk.h>
#include<sqlite3.h>
#include<stdlib.h>
#include<string.h>

#include<assert.h>

enum
  {
    COL_NAME = 0,
    COL_AGE,
    NUM_COLS
  } ;

void
list_store_set_value_from_stmt (GtkListStore* store, GtkTreeIter* iter_t, sqlite3_stmt* stmt, int column)
{
  int n;
  double d;
  const unsigned char* str;
  void* value = NULL;

  /* n = (int*) malloc(sizeof(int)); */

  int type = sqlite3_column_type(stmt, column);

  switch(type){
  case SQLITE_INTEGER:
    n = sqlite3_column_int(stmt, column);
    break;
  case SQLITE_FLOAT:
    d = sqlite3_column_double(stmt, column);
    break;
  case SQLITE_TEXT:
    str = sqlite3_column_text(stmt, column);
    n = sqlite3_column_bytes(stmt, column);
    value = (char *) str;
    break;
  case SQLITE_BLOB:
    value = "BLOB";
    break;
  case SQLITE_NULL:
    value = "NULL";
    break;
  default:
    printf("Unknown type.\n");
    value = "UNKNOWN";
    break;
  }
  if(value == NULL){            /* never happen? */
    gtk_list_store_set(store, iter_t, column, n, -1);
  }else{
    gtk_list_store_set(store, iter_t, column, value, -1);
  }
}

  int
list_store_value_from_stmt (GtkListStore* store, GtkTreeIter* iter_t, sqlite3_stmt* stmt, int columns)
{
  int i;
  int rc;

  rc = sqlite3_step(stmt);
  if(rc != SQLITE_ROW){ return rc; }

  for (i = 0; i < columns; i++){
    list_store_set_value_from_stmt(store, iter_t, stmt, i);
  }

  return rc;
}

GType*
get_column_types(sqlite3_stmt* stmt)
{
  int columns = sqlite3_column_count(stmt);
  GType* a;
  int i;
  int type;

  a = (GType *) malloc(sizeof(GType) * columns);
  for(i = 0; i < columns; i++){
    type = sqlite3_column_type(stmt, i);
    /* all types are NULL untill sqlite3_step() is executed. */

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
create_and_fill_model (sqlite3_stmt* stmt)
{
  GtkListStore  *store;
  GtkTreeIter    iter;

  int columns;
  GType* types;
  int rc;

  columns = sqlite3_column_count(stmt);
  types = get_column_types(stmt);

  store = gtk_list_store_newv (columns, types);

  while(1){
    gtk_list_store_append (store, &iter);
    rc = list_store_value_from_stmt(store, &iter, stmt, columns);
    if(rc != SQLITE_ROW){ break; }
  }

  return GTK_TREE_MODEL (store);
}

void
tree_view_insert_columns_from_stmt(GtkWidget* view, sqlite3_stmt* stmt)
{
  int columns;
  int i;
  GtkCellRenderer* renderer;

  columns = sqlite3_column_count(stmt);
  for(i = 0; i < columns; i++){
    renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view),
                                                -1,
                                                "clmn",
                                                renderer,
                                                "text", i,
                                                NULL);
  }
}

static GtkWidget *
create_view_and_model (sqlite3_stmt* stmt)
{
  /* GtkCellRenderer     *renderer; */
  GtkTreeModel        *model;
  GtkWidget           *view;

  view = gtk_tree_view_new ();

  tree_view_insert_columns_from_stmt(view, stmt);

  /* /\* --- Column #1 --- *\/ */

  /* renderer = gtk_cell_renderer_text_new (); */
  /* gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view), */
  /*                                              -1, */
  /*                                              "Name", */
  /*                                              renderer, */
  /*                                              "text", COL_NAME, */
  /*                                              NULL); */

  /* /\* --- Column #2 --- *\/ */

  /* renderer = gtk_cell_renderer_text_new (); */
  /* gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (view), */
  /*                                              -1, */
  /*                                              "Age", */
  /*                                              renderer, */
  /*                                              "text", COL_AGE, */
  /*                                              NULL); */

  model = create_and_fill_model (stmt);

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

const unsigned char* get_tablename(sqlite3* db, sqlite3_stmt* stmt){
  const unsigned char* str;

  int type = sqlite3_column_type(stmt, 0);
  assert(type == SQLITE_TEXT);
  str = sqlite3_column_text(stmt, 0);
  return str;
}

char**
get_tables(sqlite3* db)
{
  int max = 20;
  int i;
  char** a;

  int rc;
  sqlite3_stmt* stmt;
  char* query = "SELECT name FROM sqlite_master WHERE type='table'"
    "UNION ALL SELECT name FROM sqlite_temp_master WHERE type='table'"
    "ORDER BY name;";

  a = (char **) malloc(sizeof(char *) * max);

  rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
  assert(! rc);
  for (i = 0; i < max; i++){
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_ROW){ break; }
    a[i] = strdup((char *)get_tablename(db, stmt));
  }
  sqlite3_finalize(stmt);
  return a;
}

int prepare_get_records(sqlite3* db, char* table, sqlite3_stmt** stmt_t)
{
  char* buf;
  int len = 128;
  int n;
  int rc;

  buf = (char *)malloc(sizeof(char) * len);
  n = snprintf(buf, sizeof(char) * len, "select * from %s;", table);
  assert(0 <= n && n < len - 1); /* NULL is not included in n */

  rc = sqlite3_prepare_v2(db, buf, -1, stmt_t, NULL);
  return rc;
}

void
create_cells_window (char* filename)
{
  GtkWidget *window;
  GtkWidget *scrolled_window;
  GtkWidget *view;

  sqlite3* db;
  sqlite3_stmt* stmt;
  int rc;
  char** tables;

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  g_signal_connect (window, "delete_event", gtk_main_quit, NULL); /* dirty */

  scrolled_window = create_scrolled_window();
  gtk_container_add (GTK_CONTAINER (window), scrolled_window);
  /* this is done with gtk_widget_show_all? */
  /* gtk_widget_show (scrolled_window); */

  rc = sqlite3_open_v2(filename, &db, SQLITE_OPEN_READONLY, NULL);
  assert(! rc);

  tables = get_tables(db);

  int tn = 3;
  printf("%s\n", tables[tn]);
  rc = prepare_get_records(db, tables[tn], &stmt);

  view = create_view_and_model(stmt);

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
