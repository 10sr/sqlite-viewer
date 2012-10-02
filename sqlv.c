#include<stdio.h>
#include<assert.h>
#include<stdlib.h>

#include<sqlite3.h>

void check_err(sqlite3* db){
  int status = sqlite3_errcode(db);

  /* printf("status: %d.\n", status); */
  if(status == SQLITE_OK){ return; }

  /* printf("cherr: %s\n", sqlite3_errmsg(db)); */
  /* exit(status); */

  return;
}

void print_column(int ix, sqlite3* db, sqlite3_stmt* stmt){
  int n;
  double d;
  const unsigned char* str;

  int type = sqlite3_column_type(stmt, ix);

  switch(type){
  case SQLITE_INTEGER:
    printf("INT   ");
    n = sqlite3_column_int(stmt, ix);
    check_err(db);
    printf("%d\n", n);
    break;
  case SQLITE_FLOAT:
    printf("FLOAT ");
    d = sqlite3_column_double(stmt, ix);
    check_err(db);
    printf("%e\n", d);
    break;
  case SQLITE_TEXT:
    printf("TEXT  ");
    str = sqlite3_column_text(stmt, ix);
    check_err(db);
    n = sqlite3_column_bytes(stmt, ix);
    check_err(db);
    printf("%s\n", str);
    break;
  case SQLITE_BLOB:
    printf("BLOB  ");
    printf("\n");
    break;
  case SQLITE_NULL:
    printf("NULL  ");
    printf("\n");
    break;
  default:
    printf("Unknown type.\n");
    break;
  }
}

void print_row(int num, sqlite3* db, sqlite3_stmt* stmt){
  int i = 0;
  int n = sqlite3_column_count(stmt);

  printf("Row %d:\n", num);

  for(i = 0; i < n; i++){
    print_column(i, db, stmt);
  }
  printf("\n");
}

void gen_query(char* buf, int len, char* tablename){
  int n = snprintf(buf, sizeof(char) * len, "select * from %s;", tablename);
  assert(0 <= n && n < len - 1); /* NULL is not included in n */
  return;
}

const unsigned char* get_tablename(sqlite3* db, sqlite3_stmt* stmt){
  const unsigned char* str;

  int type = sqlite3_column_type(stmt, 0);
  assert(type == SQLITE_TEXT);
  str = sqlite3_column_text(stmt, 0);
  return str;
}

void print_tables(sqlite3* db){
  int rc;
  sqlite3_stmt* stmt;
  char* query = "SELECT name FROM sqlite_master WHERE type='table'"
    "UNION ALL SELECT name FROM sqlite_temp_master WHERE type='table'"
    "ORDER BY name;";
  const unsigned char* tablename;

  rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
  assert(! rc);
  while(1){
    printf("%d ", 0);
    rc = sqlite3_step(stmt);
    printf("%d ", 1);
    if(rc != SQLITE_ROW){ break; }
    printf("%d ", 2);
    tablename = get_tablename(db, stmt);
    printf("%d ", 3);
    printf("%s\n", tablename);
    printf("%d ", 4);
  }
  return;
}

void run(char* fname, char* tablename){
  int rc;
  int i = 0;
  int len = 1000;
  char buf[len];

  sqlite3* db;
  sqlite3_stmt* stmt;

  rc = sqlite3_open_v2(fname, &db, SQLITE_OPEN_READONLY, NULL);
  /* check_err(db); */
  assert(! rc);

  print_tables(db);

  if(tablename != NULL){

    gen_query(buf, len, tablename);

    rc = sqlite3_prepare_v2(db, buf, -1, &stmt, NULL);
    /* check_err(db); */
    assert(! rc);

    for(i = 0; i < 5; i++){
      rc = sqlite3_step(stmt);
      if(rc != SQLITE_ROW){ break; } /* SQLITE_DONE returns no row as well */
      print_row(i, db, stmt);
    }

  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return;
}

int main(int argc, char** argv){
  if(argc <= 1){
    fprintf(stderr, "1 argument required!\n");
    return 1;
  }else if(argc == 2){
    run(argv[1], NULL);
  }else if(argc == 3){
    run(argv[1], argv[2]);
  }

  return 0;
}
