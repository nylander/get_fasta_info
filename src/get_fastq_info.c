/*        
*          File: get_fastq_info.c
*            By: Johan Nylander
* Last modified: Wed Jan 29, 2020  12:30PM
*   Description: Get min/max/avg sequence length in fastq.
*                Can read compressed (gzip) files.
*                Prints to both stdout and stderr.
*       Compile: gcc -Wall -o get_fastq_info get_fastq_info.c -lm -lz
*           Run: get_fastq_info fastq.fq.gz
*/

#include <ctype.h>
#include <libgen.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <zlib.h>

int main (int argc, char **argv) {

    char *fname;
    char r; // r is the character currently read
    extern char *optarg;
    extern int optind;
    float avelen = 0.0f;
    int c;
    int err = 0;
    int linecounter;
    int verbose = 1;
    long int maxlen;
    long int minlen;
    long int nseqs;
    long int seqlen;
    long int sum;

    static char usage[] = "\nGet basic summary info about fastq formatted files.\n\nUsage:\n\n %s [-h][-n] infile(s).\n\n  -h is help\n  -n is noverbose\n\n  infile should be in fastq format (gzipped or not).\n\n";

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
            nseqs = 0;
            seqlen = 0;
            sum = 0;
            linecounter = 0;

            while ((r = gzgetc(zfp)) != EOF) {
                if (r == '\n') {
                    ++linecounter;
                    if (linecounter == 4) {
                        if (seqlen > 0) {
                            if (seqlen > maxlen) {
                                maxlen = seqlen;
                            }
                            if (seqlen < minlen) {
                                minlen = seqlen;
                            }
                            sum += seqlen;
                        }
                        ++nseqs;
                        linecounter = 0;
                        seqlen = 0;
                    }
                }
                if (linecounter == 1) {
                    if (! isspace(r)) {
                        ++seqlen;
                    }
                }
            }

            avelen = 1.0 * sum / nseqs;

            if (verbose) {
                fprintf(stderr, "%s", "Nseqs\tMin.len\tMax.len\tAvg.len\tFile\n");
            }

            fprintf(stdout, "%ld\t%ld\t%ld\t%g\t%s\n", nseqs, minlen, maxlen, round(avelen), basename(fname));

            gzclose(zfp);

        }
    }
    else {
        printf("Error: Need input fastq file(s) to process.\n");
        fprintf(stderr, usage, argv[0]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;

}

