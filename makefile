default : sqlv cells

sqlv:
	gcc -Wall -g `pkg-config sqlite3 --cflags --libs`  sqlv.c -o sqlv

cells:
	gcc -Wall -g cells.c -o cells `pkg-config gtk+-2.0 --cflags --libs`
