default : all

all : sqlv cells

sqlv: sqlv.c
	gcc -Wall -g sqlv.c -o sqlv `pkg-config sqlite3 --cflags --libs`

cells: cells.c
	gcc -Wall -g cells.c -o cells `pkg-config gtk+-2.0 sqlite3 --cflags --libs`

