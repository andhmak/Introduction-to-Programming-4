/* File: sorting.c */

#include <string.h>

void swapvaluei(int *v1, int *v2) {     /* Function for swapping the values of two int variables */
	int temp;
	temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}

void swapvaluec(char *v1, char *v2) {   /* Function for swapping the values of two char variables */
	char temp;
	temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}

void swaparrays(char **v1, char **v2) { /* Function for swapping the contents of two character arrays */
	char *temp;
	temp = *v1;
	*v1 = *v2;
	*v2 = temp;
}

/* Function to swap all the information of 2 countries */

void swapcountriesfull(int *nneighb, int a, int b, char **neighb, char **name, char *color, char *locked, int ncountries, int *ncolneighb) {
	int i;
	swapvaluei(&nneighb[a], &nneighb[b]);         /* Swap number of neighbours */
	swapvaluec(&color[a], &color[b]);             /* Swap colors */
	swapvaluec(&locked[a], &locked[b]);           /* Swap "locked" status */
	for (i = 0 ; i < ncountries ; i++) {          /* Swap others' neighbouring status toward them */
		swapvaluec(&neighb[i][a], &neighb[i][b]);
	}
	swaparrays(&neighb[a], &neighb[b]);           /* Swap neighbour list */
	swaparrays(&name[a], &name[b]);               /* Swap names */
	swapvaluei(&ncolneighb[a], &ncolneighb[b]);   /* Swap number of colored neighbours */
}

/* Sorting function putting after each country the country with the most previous neighbours */

void neighbsort(int *nneighb, char **neighb, char **name, char *color, char *locked, int ncountries) {
	/* Variables */
	/* ncolneighb: number of neighbours that will be colored before country, max: maximum value of previous array, maxp: position of the maximum value */
	int i, j, ncolneighb[ncountries], max, maxp;
	for (i = 0 ; i < ncountries ; i++) {         /* Initialise ncolorneighb array */
		ncolneighb[i] = 0;
	}
	for (i = ncountries - 1 ; i > 0 ; i--) {     /* For every country, which will be colored */
		for (j = i - 1 ; j >= 0 ; j--) {         /* Check every other contry after it */
			if (neighb[i][j]) {                  /* If they border each other, increase the number of colored neighbours of the latter by 1 */
				ncolneighb[j]++;
			}
		}
		max = 0;                                 /* Initialise maximum number of neighbours to 0 */
		for (j = 0 ; j <= i - 1 ; j++) {         /* Find the last country that has the most colored neighbours */
			if (ncolneighb[j] >= max) {
				max = ncolneighb[j];
				maxp = j;
			}
		}
		swapcountriesfull(nneighb, i - 1, maxp, neighb, name, color, locked, ncountries, ncolneighb); /* Make it the next one to be colored */
	}
}

char arraySorted(char **array, int ncountries) {  /* Function to check is a given string array is sorted */
	int i;
    if (ncountries == 0 || ncountries == 1) {     /* If the array has one or zero elements */
        return 1;                                 /* It's considered sorted */
    }
    for (i = 1 ; i < ncountries ; i++) {
        if (strcmp(array[i - 1], array[i]) > 0) { /* If an unsorted pair is found */
            return 0;                             /* Return value indicating the array isn't sorted */
        }
    }
    return 1;                                     /* If no unsorted pair found, it's sorted */
}

/* Function to swap part of the information of 2 countries, along with the lines of text from the input associated with them not yet processed */

void swapcountriespart(int a, int b, char **name, char *locked, char *color, char **linetext) {
	swapvaluec(&locked[a], &locked[b]);        /* Swap "locked" status */
	swapvaluec(&color[a], &color[b]);          /* Swap colors */
	swaparrays(&name[a], &name[b]);            /* Swap names */
	swaparrays(&linetext[a], &linetext[b]);    /* Swap lines of text */
}

/* Traditional heapsort/heapify functions (like the ones in the lecture notes), sorting the (partial) countries based on a given string array, which will be "name" in this case */

void heapifyalph(char **name, int root, int bottom, char *locked, char *color, char **linetext) {
	int maxchild;
	while (2*root < bottom) {
		if (2*root + 1 == bottom) {
			maxchild = 2*root + 1;
		}
		else if (strcmp(name[2*root + 1], name[2*root + 2]) > 0) {
			maxchild = 2*root + 1;
		}
		else {
			maxchild = 2*root + 2;
		}
		if (strcmp(name[maxchild], name[root]) > 0) {
			swapcountriespart(maxchild, root, name, locked, color, linetext);
			root = maxchild;
		}
		else {
			break;
		}
	}
}

void heapsortalph(char **name, int ncountries, char *locked, char *color, char **linetext) {
	int i;
	for (i = (ncountries/2) - 1 ; i >= 0 ; i--) {
		heapifyalph(name, i, ncountries - 1, locked, color, linetext);
	}
	for (i = ncountries - 1 ; i >= 1 ; i--) {
		swapcountriespart(0, i, name, locked, color, linetext);
		heapifyalph(name, 0, i - 1, locked, color, linetext);
	}
}
