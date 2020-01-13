/*        
*          File: get_fasta_info.c
*            By: Johan Nylander
* Last modified: mån jan 13, 2020  11:52
*   Description: Get min/max/avg sequence length in fasta.
*                Can read compressed (gzip) files.
*                Prints to both stdout and stderr.
*       Compile: gcc -Wall -o get_fasta_info get_fasta_info.c -lm -lz
*           Run: get_fasta_info fasta.fas
*/

#include <ctype.h>
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
    float avelen;
    char r; // r is the character currently read
    int inheader;
    int verbose = 1;
    char *fname;
    extern char *optarg;
    extern int optind;
    int c, err = 0;

    static char usage[] = "\nGet basic summary info about fasta formatted files.\n\nUsage:\n\n %s [-h][-n] infile(s).\n\n  -h is help\n  -n is noverbose\n\n  infile should be in fasta format.\n\n";

    if (argc == 1) {
        fprintf(stderr, usage, argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((c = getopt(argc, argv, "hn")) != -1) {
        switch (c) {
            case 'h':
                fprintf(stderr, usage, argv[0]);
                exit(1);
                break;
            case 'n':
                verbose = 0;
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
                    inheader = 1;
                    if (nseqs > 0) {
                        if (seqlen > 0) {
                            if (seqlen > maxlen) {
                                maxlen = seqlen;
                            }
                            if (seqlen < minlen) {
                                minlen = seqlen;
                            }
                            sum += seqlen;
                        }
                        else {
                            minlen = 0;
                        }
                        seqlen = 0;
                    }
                    ++nseqs;
                }
                else {
                    if (!isspace(r)) {
                        ++seqlen;
                    }
                }
            }

            // Take care of last seqlen
            if (seqlen > maxlen) {
                maxlen = seqlen;
            }
            if (seqlen < minlen) {
                minlen = seqlen;
            }
            sum += seqlen;

            // If all empty sequences
            if (minlen == INT_MAX && maxlen == INT_MIN) {
                minlen = maxlen = 0;
            }

            if (sum > 0) {
                avelen = 1.0 * sum / nseqs;
            }
            else {
                avelen = 0;
            }

            if (verbose) {
                fprintf(stderr, "%s", "Nseqs\tMin.len\tMax.len\tAvg.len\tFile\n");
            }
            fprintf(stdout, "%ld\t%ld\t%ld\t%g\t%s\n", nseqs, minlen, maxlen, round(avelen), basename(fname));

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

    return EXIT_SUCCESS;

}

