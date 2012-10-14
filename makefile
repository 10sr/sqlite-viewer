default : all

all : sqlv cells cellsql

sqlv: sqlv.c
	gcc -Wall -g sqlv.c -o sqlv `pkg-config sqlite3 --cflags --libs`

cells: cells.c
	gcc -Wall -g cells.c -o cells `pkg-config gtk+-2.0 sqlite3 --cflags --libs`

cellsql: cellsql.o psqlite.o
	gcc -Wall -g cellsql.o psqlite.o -o cellsql `pkg-config gtk+-2.0 sqlite3 --libs`

cellsql.o : cellsql.c psqlite.h
	gcc -Wall -g -c cellsql.c -o cellsql.o `pkg-config gtk+-2.0 --cflags`

psqlite.o : psqlite.c psqlite.h
	gcc -Wall -g -c psqlite.c -o psqlite.o `pkg-config sqlite3 --cflags`
