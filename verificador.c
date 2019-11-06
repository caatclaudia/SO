/* verificador.c  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


/* maximum word def */
#define MAXNW 50
/* maximum word length including \0 */
#define MAXWL 19

/* This function does what its name tells */
void exitNow(int s) {
    exit(0);
}

/* reads forbidden word list from file */
/* assume all words have at most 19 chars */
/* file must not have more than one word per line */
/* return number of word read */
int readWordFile(FILE * f, char wdef[][MAXWL], int maxw) {
    int numw = 0;
    while (!feof(f) && numw < maxw)
        if (fscanf(f,"%s", wdef[numw])>0)
            numw++;
    return numw;
}

/* checks word agains forbidden word dictionary */
/* word must match exactly - partials do not count */
/* returns 0 or 1*/
int checkWord(char * word, char wdef[][MAXWL], int maxw) {
    int i;
    for (i=0; i<maxw;i++)
        if (!strcmp(word,wdef[i]))
            return 1;
    return 0;
}

/* The main fuction. It's kind of useful to exist */
int main(int argc, char** argv) {
    char forbwords[MAXNW][MAXWL];
    char word[MAXWL];
    int numw;  /* number of words */
    int numhits;  /* hits in this message */
    int justprinted;
    FILE * wordsf;
    if (argc<2) {
        printf("ERROR-1\n");
        return 1;
    }
    wordsf = fopen(argv[1], "r");
    if (wordsf == NULL) {
        printf("ERROR-2\n");
        return 2;
    }
    numw = readWordFile(wordsf, forbwords, MAXNW);
    fclose(wordsf);
    if (numw < 1) {
        printf("ERROR-3\n");
        return 3;
    }
    signal(SIGUSR2, exitNow);
    /* preps done. start text processing */
    numhits = 0;
    justprinted = 0;
    while (!feof(stdin)) {
        if (scanf("%s", word)<0)
            continue;
        if (!strcmp(word,"##MSGEND##")) {
            printf("%d\n",numhits);
            fflush(stdout);
            numhits = 0; /* restart counting */
            justprinted = 1;
        }
        else {
            numhits += checkWord(word, forbwords, numw);
            justprinted = 0;
        }
    }
    if (!justprinted)
        printf("%d\n",numhits);
    return 0;
}
