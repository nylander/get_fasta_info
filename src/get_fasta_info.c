/*        
*          File: get_fasta_info.c
*            By: Johan Nylander
* Last modified: fre jan 10, 2020  03:08
*   Description: Get min/max/avg sequence length in fasta.
*                Can read compressed (gzip) files.
*                Prints to both stdout and stderr.
*       Compile: gcc -Wall -o get_fasta_info get_fasta_info.c -lm -lz
*           Run: get_fasta_info fasta.fas
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include <math.h>
#include <zlib.h>

int main (int argc, char **argv) {

    long int minlen;
    long int maxlen;
    long int sum;
    long int seqlen;
    long int nseqs;
    float meanlen;
    char r; // r is the character currently read
    int inheader;
    char *fname;
    extern char *optarg;
    extern int optind;
    int c, err = 0;

    static char usage[] = "Usage: %s [-h] infile(s)\n";

    if (argc == 1) {
        fprintf(stderr, usage, argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((c = getopt(argc, argv, "h")) != -1) {
        switch (c) {
            case 'h':
                fprintf(stderr, usage, argv[0]);
                exit(1);
                break;
            case '?':
                err = 1;
                break;
        }
    }

    if (err) {
        fprintf(stderr, usage, argv[0]);
        exit(1);
    }

    if (optind < argc) {

        for (; optind < argc; optind++) {

            gzFile zfp = gzopen(argv[optind], "rb");

            if (zfp == NULL) {
                perror("Error: failed in opening file");
                exit(EXIT_FAILURE);
            }

            fname = argv[optind];
            minlen = INT_MAX;
            maxlen = INT_MIN;
            inheader = 0;
            nseqs = 0;
            seqlen = 0;
            sum = 0;

            while ((r = gzgetc(zfp)) != EOF) {
                if (inheader == 1) {
                    if (r == '\n') {
                        inheader = 0;
                    }
                }
                else if (r == '>') {
                    ++nseqs;
                    inheader = 1;
                    if (seqlen > 0) {
                        if (seqlen > maxlen) {
                            maxlen = seqlen;
                        }
                        if (seqlen < minlen) {
                            minlen = seqlen;
                        }
                        sum += seqlen;
                        seqlen = 0;
                    }
                }
                else {
                    if (r != '\n') {
                        ++seqlen;
                    }
                }
            }

            // Take care of last seqlen
            if (seqlen > 0) {
                if (seqlen > maxlen) {
                    maxlen = seqlen;
                }
                if (seqlen < minlen) {
                    minlen = seqlen;
                }
                sum += seqlen;
            }

            meanlen = 1.0 * sum / nseqs;

            fprintf(stderr, "%s", "Nseqs\tMin.len\tMax.len\tAvg.len\tFile\n");
            fprintf(stdout, "%ld\t%ld\t%ld\t%g\t%s\n", nseqs, minlen, maxlen, round(meanlen), basename(fname));

            minlen = INT_MAX;
            maxlen = INT_MIN;
            nseqs = 0;
            seqlen = 0;
            sum = 0;

            gzclose(zfp);

        }
    }
    else {
        printf("Error: Need input fasta file(s) to process.\n");
        fprintf(stderr, usage, argv[0]);
        exit(EXIT_FAILURE);
    }

    exit(0);

}

