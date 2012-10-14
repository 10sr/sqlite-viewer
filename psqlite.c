#include"psqlite.h"

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>

void psqlite_table_get_columns(psqlite_table* tb);
void psqlite_table_put_record(psqlite_table* tb, psqlite_data** a);

void psqlite_connect(char* filename, psqlite** db){
  psqlite* new;

  new = malloc(sizeof(psqlite));
  assert(new != NULL);

  new->filename = strdup(filename);
  new->rc = sqlite3_open_v2(filename, &(new->con), SQLITE_OPEN_READONLY, NULL);

  *db = new;

  return;
}

void psqlite_get_tables_name(psqlite* db){
  int max = 20;                 /* use realloc! */
  int i;
  char** a;

  sqlite3_stmt* stmt;
  char* query = "SELECT name FROM sqlite_master WHERE type='table'"
    "UNION ALL SELECT name FROM sqlite_temp_master WHERE type='table'"
    "ORDER BY name"
    ";";
  int type;

  a = (char **) malloc(sizeof(char *) * max);
  assert(a != NULL);

  db->rc = sqlite3_prepare_v2(db->con, query, -1, &stmt, NULL);
  assert(! db->rc);
  for (i = 0; i < max; i++){
    db->rc = sqlite3_step(stmt);
    if(db->rc != SQLITE_ROW){ break; }
    type = sqlite3_column_type(stmt, 0);
    assert(type == SQLITE_TEXT);
    a[i] = strdup((char *)sqlite3_column_text(stmt, 0));
  }
  sqlite3_finalize(stmt);

  db->tables = a;
  db->table_num = i;
}

void psqlite_get_table(psqlite* db, char* tablename, psqlite_table** tb){
  int len = 256;                /* use realloc!! */
  char* query;
  int n;

  psqlite_table* new;

  new = malloc(sizeof(psqlite_table));
  assert(new != NULL);
  new->name = strdup(tablename);
  new->db = db;
  new->stmt = NULL;

  /* generate query */
  query = malloc(sizeof(char) * len);
  assert(query != NULL);
  n = snprintf(query, sizeof(char) * len, "select * from %s;", tablename);
  assert(0 <= n && n < len - 1); /* NULL is not included in n */

  new->query = query;

  psqlite_table_get_columns(new);

  *tb = new;

  return;
}

void psqlite_table_get_columns(psqlite_table* tb){
  char** a;
  int max = 20;                 /* use realloc!!! */
  int n, i;

  int len = 256;                /* use realloc!!!! */
  char* query;
  sqlite3_stmt* stmt;
  int type;

  a = (char **) malloc(sizeof(char *) * max);
  assert(a != NULL);

  /* generate query */
  query = malloc(sizeof(char) * len);
  assert(query != NULL);
  n = snprintf(query, sizeof(char) * len, "PRAGMA TABLE_INFO(%s);", tb->name);
  assert(0 <= n && n < len - 1); /* NULL is not included in n */

  tb->rc = sqlite3_prepare_v2(tb->db->con, query, -1, &stmt, NULL);
  assert(! tb->rc);
  for (i = 0; i < max; i++){
    tb->rc = sqlite3_step(stmt);
    if(tb->rc != SQLITE_ROW){ break; }
    type = sqlite3_column_type(stmt, 0);
    assert(type == SQLITE_TEXT);
    a[i] = strdup((char *)sqlite3_column_text(stmt, 0));
  }
  sqlite3_finalize(stmt);

  tb->columns = a;
  tb->column_num = i;
}

void psqlite_query(psqlite* db, char* query, psqlite_table** tb){
  return;
}

void psqlite_table_fetch(psqlite_table* tb, int n){
  int i;
  psqlite_data** aa;

  aa = malloc(sizeof(psqlite_data*) * n);
  assert(aa != NULL);

  if (tb->stmt == NULL){
    tb->rc = sqlite3_prepare_v2(tb->db->con, tb->query, -1, &(tb->stmt), NULL);
  }
  assert(! tb->rc);

  for(i = 0; i < n; i++){
    tb->rc = sqlite3_step(tb->stmt);
    if (tb->rc != SQLITE_ROW) { break; }
    psqlite_table_put_record(tb, &(aa[i]));
  }

  tb->data_num = i;
  tb->data = aa;

  return;
}

void psqlite_table_put_record(psqlite_table* tb, psqlite_data** a){
  int i;
  psqlite_data* new;
  int columns = tb->column_num;

  new = (psqlite_data*) malloc(sizeof(psqlite_data) * columns);

  for (i = 0; i < columns; i++) {
    new[i].type = sqlite3_column_type(tb->stmt, i);

    switch(new[i].type){
    case SQLITE_INTEGER:
      new[i].val.n = sqlite3_column_int(tb->stmt, i);
      break;
    case SQLITE_FLOAT:
      new[i].val.d = sqlite3_column_double(tb->stmt, i);
      break;
    case SQLITE_TEXT:
      new[i].val.s = strdup((char*) sqlite3_column_text(tb->stmt, i));
      /* n = sqlite3_column_bytes(stmt, i); */
      break;
    case SQLITE_BLOB:
      break;
    case SQLITE_NULL:
      break;
    default:
      /* printf("Unknown type.\n"); */
      break;
    }
  }

  *a = new;

  return;
}
