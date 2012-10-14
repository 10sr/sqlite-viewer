#include<sqlite3.h>

/* psqlite */

struct _psqlite {
  char* filename;
  char** tables;
  int table_num;
  sqlite3* con;
  int rc;
};

typedef struct _psqlite psqlite;

void psqlite_connect(char* filename, psqlite** db);

void psqlite_get_tables_name(psqlite* db);

/* void psqlite_destroy(psqlite* db); */

/* sqlite_data */

struct _psqlite_data {
  int type;
  union {
    int n;
    char* s;
    double d;
  } val;
};

typedef struct _psqlite_data psqlite_data;
/* psqlite_table */

struct _psqlite_table {
  char* name;
  char* query;
  char** columns;
  int column_num;
  psqlite_data** data;
  int data_num;
  psqlite* db;
  sqlite3_stmt* stmt;
  int rc;
};

typedef struct _psqlite_table psqlite_table;

void psqlite_get_table(psqlite* db, char* tablename, psqlite_table** tb);

void psqlite_query(psqlite* db, char* query, psqlite_table** tb);

void psqlite_table_fetch(psqlite_table* tb, int n);
