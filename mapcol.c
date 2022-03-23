/* File: mapcol.c */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sorting.h"
#include "coloring.h"
#define LINEBUFFER 32768  /* The line and word buffers can be enlarged to handle larger maps */
#define WORDBUFFER 32
#define COLORBUFFER 8
#define NCOLORS 10        /* The maximum number of colors the program can handle can also be changed by changing this value along with adding colors in "colorstr" */

char *colorstr[] = {"nocolor", "red", "green", "blue", "yellow", "orange",  /* Array storing all color names for input/output */
					"violet", "cyan", "pink", "brown", "grey"};

void *errmalloc(unsigned int size) {         /* The usual "malloc" but also checking for possible error in  memory allocation */
	void *p;
	if ((p = malloc(size)) == NULL) {
		perror("Memory allocation");
		exit(-1);
	}
	return p;
}

struct line {                                /* Self referential structure storing each input line for map initialisation */
	char *text;
	struct line *next;
};

/* Variables */
/* linebuf: buffer for reading lines of text for input, wordbuf: buffer for input words, colorbuf: buffer specifically for reading colors */
/* neighb: matrix for storing a binary value of whether two countries share a border (if countries i, j border each other then neighb[i][j] == 1 and neighb[j][i] == 1 */
/* name: string array storing the name of each country, color: char array storing the color of each country as a numerical value from 0 to NCOLORS */
/* locked: char array storing a binary value of whether the country's color is preset/locked, ncolors: maximum number of colors allowed */
/* text_orig: string array storing the text of each line, because the pointers in the struct will be moved, and we will need to free the space allocated for the text */
/* linetext: array storing the contents of each line/the line structure list turned into an array, so that it can be sorted more easily */
/* i, j: for loops, verify_mode: whether the program colors or verifies maps (0/1 respectively), ncountries: number of countries in given map */
/* nneighb: int array storing the number of neighbours of each country, left, right: to be used as "limits" in minary search */
/* cmpnum: stores the value returned by strcmp, input: pointer to input file, start: pointer to the first line structure node, used to access the list */
/* current: pointer to node, used to initialise and traverse the list */

int main(int argc, char **argv) {
	char linebuf[LINEBUFFER] = "", wordbuf[WORDBUFFER] = "", colorbuf[COLORBUFFER] = "", **neighb, **name, *locked, *color, ncolors = 4, **text_orig, **linetext;
	int i, j, verify_mode = 0, ncountries = 0, *nneighb, left, right, cmpnum;
	FILE *input = stdin;
	struct line *start = NULL, *current = NULL;
	if (argc > 6) {                                           /* There can't be more than 6 arguments (./mapcol -n <num> -i "file" -c) */
		fprintf(stderr, "Wrong usage\n");
		return 1;
	}
	if (!(strcmp(*(argv + argc - 1), "-i")) || !(strcmp(*(argv + argc - 1), "-n"))) { /* The last argument can't be -n or -c */
		fprintf(stderr, "Wrong usage\n");
		return 1;
	}
	for (i = 1 ; i < argc ; i++) {                            /* If an argument is "-c" enter verification mode */
		if (!strcmp(*(argv + i), "-c")) {
			verify_mode = 1;
			break;
		}
	}
	for (i = 1 ; i < argc - 1 ; i++) {                        /* Set the numeric value of the argument after "-n" as the maximum number of colors allowed */
		if (!strcmp(*(argv + i), "-n")) {
			ncolors = atoi(*(argv + i + 1));
			if ((ncolors <= 0) || (ncolors > NCOLORS)) {
				fprintf(stderr, "Invalid number of colors\n");
				return 2;
			}
			break;
		}
	}
	for (i = 1 ; i < argc - 1 ; i++) {                        /* Open the file specified after "-i" and set it as the input stream */
		if (!strcmp(*(argv + i), "-i")) {
			if ((input = fopen(*(argv + i + 1), "rb")) == NULL) { /* Check is it fopen was successful */
				perror("fopen source-file");
				return 3;
			}
			break;
		}
	}
	if (fgets(linebuf, LINEBUFFER, input) != NULL) {         /* Store the first line of text in the buffer */
		start = errmalloc(sizeof(struct line));              /* Allocate memory for the first line node */
		start->text = errmalloc((strlen(linebuf) + 1) * sizeof(char)); /* Allocate memory for its text */
		strcpy(start->text, linebuf);                        /* Copy text from buffer to structure */
		ncountries++;                                        /* Increment the number of countries by 1 */
		if (fgets(linebuf, LINEBUFFER, input) != NULL) {     /* Do the same for the second node */
			start->next = errmalloc(sizeof(struct line));
			start->next->text = errmalloc((strlen(linebuf) + 1) * sizeof(char));
			strcpy(start->next->text, linebuf);
			current = start->next;
			ncountries++;
		}
	}
	else {                                                   /* If there is no input print an error message */
		fprintf(stderr, "No input\n");
		return 4;
	}
	while (fgets(linebuf, LINEBUFFER, input) != NULL) {      /* Continue creating and linking nodes as such, until the end of the input is reached */
		current->next = errmalloc(sizeof(struct line));
		current->next->text = errmalloc((strlen(linebuf) + 1) * sizeof(char));
		strcpy(current->next->text, linebuf);
		current = current->next;
		ncountries++;
	}
	linetext = errmalloc(ncountries * sizeof(char *));       /* Initialise the array that will replace the list */
	for (current = start, i = 0 ; current != NULL ; current = current->next, i++) { /* Tranfer contents from list to array */
		linetext[i] = current->text;
	}
	for (current = start ; current != NULL ; current = current->next) { /* Free list */
		free(current);
	}
	name = errmalloc(ncountries * sizeof(char *));           /* Allocate memory for name array */
	color = errmalloc(ncountries * sizeof(char));            /* Allocate memory for color array */
	locked = errmalloc(ncountries * sizeof(char));           /* Allocate memory for locked (preset color) array */
	for (i = 0 ; i < ncountries ; i++) {                     /* Initialise locked array with zeroes */
		locked[i] = 0;
	}
	text_orig = errmalloc(ncountries * sizeof(char *));      /* Allocate memory to store the original pointers to the text of each line, in order for it to be freed later */
	for (i = 0 ; i < ncountries ; i++) {                     /* For each line, representing a country */
		text_orig[i] = linetext[i];                          /* Store original text location in the array */
		sscanf(linetext[i], "%s", colorbuf);                 /* Scan the first word of the line in the color buffer */
		for (j = 0 ; j <= COLORBUFFER ; j++) {               /* Move string pointer to the next word */
			if (linetext[i][j] == ' ') {
				linetext[i] += j + 1;
				break;
			}
		}
		for (j = 0 ; j <= NCOLORS ; j++) {                   /* Store the colored as a numeric value, by comparing color strings */
			if (!strcmp(colorbuf, colorstr[j])) {
				color[i] = j;
				break;
			}
		}
		if (j != 0) {                                        /* If already colored, set country color as locked */
			locked[i] = 1;
		}
		if (j == (NCOLORS + 1)) {                            /* If it doesn't match any string then the color isn't recognised */
			fprintf(stderr, "%s: not recognised color\n", colorbuf);
			return 5;
		}
		if (j > ncolors) {                                   /* If more colors than allowed are used then print said message */
			printf("More than %d colors used\n", ncolors);
			return 0;
		}
		sscanf(linetext[i], "%s", wordbuf);                  /* Scan the next word of the line (the country's name) in the word buffer */
		for (j = 0 ; j <= WORDBUFFER ; j++) {                /* Move string pointer to the next word */
			if (linetext[i][j] == ' ') {
				linetext[i] += j + 1;
				break;
			}
			if (linetext[i][j] == '\n') {
				linetext[i] += j;
				break;
			}
		}
		name[i] = errmalloc((strlen(wordbuf) + 1) * sizeof(char)); /* Allocate memory for the name of the country in the name array */
		strcpy(name[i], wordbuf);                                  /* Copy name from buffer into the name array */
	}
	if (!arraySorted(name, ncountries)) {                          /* If the name array isn't sorted */
		heapsortalph(name, ncountries, locked, color, linetext);   /* Sort the countries by name */
	}
	neighb = errmalloc(ncountries * sizeof(char *));       /* Allocate memory for neighbour/border matrix */
	for (i = 0 ; i < ncountries; i++) {
    	neighb[i] = errmalloc(ncountries * sizeof(char));
	}
	for (i = 0 ; i < ncountries ; i++) {                   /* Initialise it with zeroes */
    	 for (j = 0 ; j < ncountries ; j++) {
    		neighb[i][j] = 0;
    	}
	}
	for (i = 0 ; i < ncountries ; i++) {                   /* For each line/country */
		while (sscanf(linetext[i], "%s", wordbuf) > 0) {   /* Keep scanning words, corresponding to the names of neighbouring countries, until the end of the line */
			for (j = 0 ; j <= WORDBUFFER ; j++) {
				if (linetext[i][j] == ' ') {
					linetext[i] += j + 1;
					break;
				}
				if (linetext[i][j] == '\n') {
					linetext[i] += j;
					break;
				}
			}
			if (!strcmp(wordbuf, name[i])) {                /* If the country's name appears among its neighbours, then print the equivalent message */
				fprintf(stderr, "Country bordering itself: %s\n", name[i]);
				return 6;
			}
			left = i + 1, right = ncountries - 1;           /* Find the neighbouring country, using a binary search on the sorted name array, and set them as neighbours */
			while (left <= right) {
				j = (left + right) / 2;
				cmpnum = strcmp(name[j], wordbuf);
				if (!cmpnum) {
					neighb[i][j] = 1;
					neighb[j][i] = 1;
					break;
				}
				if (cmpnum < 0) {
					left = j + 1;
				}
				else {
					right = j - 1;
				}
    		} 
		}
	}
	if (input != stdin) {                         /* If input stream is connected to a file */
		fclose(input);                            /* Close the stream */
	}
	for (i = 0 ; i < ncountries ; i++) {          /* Free the input line strings and the array storing the pointers the first character of each string */
		free(text_orig[i]);
	}
	free(text_orig);
	free(linetext);                               /* Free the array storing the modified pointers */
	if (verify_mode) {                            /* If in "-c"/"verification" mode */
		for (i = 0 ; i < ncountries ; i++) {      /* For every country */
			if (color[i] == 0) {                  /* Check if it's colored */
				printf("Please, color the map first\n");
				break;
			}
			for (j = 0 ; j < ncountries ; j++) {  /* Check if it shares a color with a neighbouring country */
				if ((neighb[i][j] == 1) && (color[i] == color[j])) {
					printf("%s - %s: Should have different color\n", name[i], name[j]);
				}
			}
		}
		return 0;                                 /* Exit program without errors */
	}
	nneighb = malloc(ncountries * sizeof(int *)); /* Initialise neighbour number array */
	for (i = 0 ; i < ncountries ; i++) {          /* For each country */
		nneighb[i] = 0;                           /* Initialise number of neighbours to 0 */
		for (j = 0 ; j < ncountries ; j++) {      /* Add 1 for every neighbour */
			if (neighb[i][j]) {
				nneighb[i]++;
			}
		}
	}
	neighbsort(nneighb, neighb, name, color, locked, ncountries); /* Sort the countries so that each country is followed by the countriy with most neighbours before it (starting from the end) */
	if (!coloring(neighb, ncolors, color, ncountries - 1, ncountries, locked)) { /* Color the map staring from the end */
		printf("Sorry, cannot color the given map\n");                           /* If no valid combination is found, exit after writing it's impossible to color the map */
		return 0;
	}
	for (i = 0 ; i < ncountries ; i++) {            /* Print out the map */
		printf("%s %s", colorstr[color[i]], name[i]);
		for (j = 0 ; j < ncountries ; j++) {
			if (neighb[i][j] == 1) {
				printf(" %s", name[j]);
			}
		}
		printf("\n");
	}
	return 0;
}
