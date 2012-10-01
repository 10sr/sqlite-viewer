default : sqlv cells

sqlv:
	gcc -Wall -g sqlv.c -o sqlv `pkg-config sqlite3 --cflags --libs`

cells:
	gcc -Wall -g cells.c -o cells `pkg-config gtk+-2.0 --cflags --libs`
