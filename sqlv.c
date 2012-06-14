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

  printf("Row %d.\n", num);

  for(i = 0; i < n; i++){
    print_column(i, db, stmt);
  }
  printf("\n");
}

int main(int argc, char** argv){
  int rc;
  int i = 0;

  sqlite3* db;
  sqlite3_stmt* stmt;

  rc = sqlite3_open_v2("Northwind.sqlite", &db, SQLITE_OPEN_READONLY, NULL);
  /* check_err(db); */
  assert(! rc);

  rc = sqlite3_prepare_v2(db, "select * from Orders;", -1, &stmt, NULL);
  /* check_err(db); */
  assert(! rc);

  for(i = 0; i < 10; i++){
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_ROW){ break; } /* SQLITE_DONE returns no row as well */
    print_row(i, db, stmt);
  }

  sqlite3_finalize(stmt);
  sqlite3_close(db);

  return 0;
}
