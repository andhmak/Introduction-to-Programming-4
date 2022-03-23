mapcol: mapcol.o coloring.o sorting.o
	gcc -o mapcol mapcol.o coloring.o sorting.o
mapcol.o: mapcol.c
	gcc -c -o mapcol.o mapcol.c
coloring.o: coloring.c
	gcc -c -o coloring.o coloring.c
sorting.o: sorting.c
	gcc -c -o sorting.o sorting.c
