/* File: coloring.c */

/* Map coloring function. Returning 0 means the relevant country couldn't be colored. Returning 1 means it and every one "after" it was colored successfully. */

char coloring(char **neighb, char ncolors, char *color, int i, int ncountries, char *locked) { /* i: last country from which to start coloring */
	char c, correct = 1;  /* c: color, correct: binary, whether all neighbouring colored countries are colored differently or not */
	int j;
	if (i == -1) {                                                             /* If all countrues colored, return 1 */
		return 1;
	}
	if (locked[i]) {                                                           /* If country color preset */
		for (j = ncountries - 1 ; j >= 0 ; j--) {                              /* Check if any neighbouring country uses the same color */
			if ((neighb[i][j]) && (color[i] == color[j])) {
				return 0;                                                      /* If so return 0 */
			}
		}
		if (coloring(neighb, ncolors, color, i - 1, ncountries, locked)) {     /* Else move to the next country */
			return 1;                                                          /* If the next country returns 1, return 1 */
		}
		return 0;                                                              /* If it returns 0, return 0 */
	}
	for (c = 1 ; c <= ncolors ; correct = 1, c++) {                            /* For every color */
		color[i] = c;                                                          /* Color country */
		for (j = ncountries - 1 ; j >= 0 ; j--) {                              /* Check if any neighbouring country uses it */
			if ((neighb[i][j]) && (color[i] == color[j])) {
				correct = 0;
				break;
			}
		}
		if (correct) {                                                         /* If not, move to the next country, else try the next color */
			if (coloring(neighb, ncolors, color, i - 1, ncountries, locked)) { /* If the next country returns 1, return 1, else, try the next color */
				return 1;
			}
		}
	}
	color[i] = 0;                                                           /* If no color fits, uncolor country */
	return 0;                                                               /* And return 0 */
}
