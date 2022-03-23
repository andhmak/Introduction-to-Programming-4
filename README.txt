NOTE
       The program initialises the map from the input, using alphabetical sorting and binary search to establish the countries' borders.
       It then sorts the countries, so that the the next country is always the one with the most neighbours before it.
       The map is then colored using a recursive funtion.
       The values of LINEBUFFER and WORDBUFFER can be increased for the program to handle very large maps.

COMPILATION INSTRUCTIONS
       To compile the program type the following commands in the command line:
            gcc -c -o sorting.o sorting.c
            gcc -c -o coloring.o coloring.c
            gcc -c -o mapcol.o mapcol.c
            gcc -o mapcol mapcol.o heapsort.o coloring.o

       Or simply type the following command instead:
            make

       The resulting program will be named "mapcol".

OPTIONS
       -i <file>
            Read input from file 
       -c
            Verify given map instead of coloring it
       -n <number>
            Color map with at most <number> colors, or verify that the map is colored with at most <number> colors when combined with the -c option
