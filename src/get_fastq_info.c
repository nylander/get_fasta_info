/*
*          File: get_fastq_info.c
*            By: Johan Nylander
* Last modified: mån apr 19, 2021  10:44
*   Description: Get min/max/avg sequence length in fastq.
*                Can read compressed (gzip) files.
*                Prints to both stdout and stderr.
*                Can report average read quality (phred, ASCII_BASE=33) score.
*       Compile: gcc -Wall -03 -funroll-loops -o get_fastq_info get_fastq_info.c -lm -lz
*           Run: get_fastq_info -q fastq.fq.gz
*       License: Copyright (c) 2019-2021 Johan Nylander
*                Permission is hereby granted, free of charge, to any person
*                obtaining a copy of this software and associated documentation
*                files (the "Software"), to deal in the Software without
*                restriction, including without limitation the rights to use,
*                copy, modify, merge, publish, distribute, sublicense, and/or
*                sell copies of the Software, and to permit persons to whom the
*                Software is furnished to do so, subject to the following
*                conditions:
*                The above copyright notice and this permission notice shall be
*                included in all copies or substantial portions of the Software.
*                THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*                EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*                OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
*                NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
*                HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*                WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*                FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
*                OTHER DEALINGS IN THE SOFTWARE.
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
    float avequal = 0.0f;
    int c;
    int err = 0;
    int linecounter;
    int q;
    int quality = 0;
    int sqsum = 0;
    int verbose = 1;
    long int maxlen;
    long int minlen;
    long int nseqs;
    long int qsum = 0;
    long int seqlen;
    long int sum;

    static char usage[] = "\nGet basic summary info about fastq formatted files.\n\nUsage:\n\n %s [-h][-n][-q] infile(s).\n\n  -h help\n  -n noverbose\n  -q show avg. read qual (ASCII_BASE=33)\n\n\n  infile should be in fastq format (gzipped or not).\n\n";

    if (argc == 1) {
        fprintf(stderr, usage, argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((c = getopt(argc, argv, "hnq")) != -1) {
        switch (c) {
            case 'h':
                fprintf(stderr, usage, argv[0]);
                exit(1);
                break;
            case 'n':
                verbose = 0;
                break;
            case 'q':
                quality = 1;
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
                        if (quality == 1) {
                            if (sqsum > 0) {
                                qsum += round( sqsum / seqlen);
                            }
                            sqsum = 0;
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
                else if (linecounter == 3) {
                    if (quality == 1) {
                        if (! isspace(r)) {
                            q = r - 33; // ASCII_BASE=33
                            sqsum += q;
                        }
                    }
                }
            }

            avelen = 1.0 * sum / nseqs;

            if (quality == 1) {
                avequal = 1.0 * qsum / nseqs;
            }

            if (verbose) {
                if (quality == 1) {
                    fprintf(stderr, "%s", "Nseqs\tMin.len\tMax.len\tAvg.len\tAvg.qual\tFile\n");
                }
                else {
                    fprintf(stderr, "%s", "Nseqs\tMin.len\tMax.len\tAvg.len\tFile\n");
                }
            }

            if (quality == 1) {
                fprintf(stdout, "%ld\t%ld\t%ld\t%g\t%g\t%s\n", nseqs, minlen, maxlen, round(avelen), round(avequal), basename(fname));
            }
            else {
                fprintf(stdout, "%ld\t%ld\t%ld\t%g\t%s\n", nseqs, minlen, maxlen, round(avelen), basename(fname));
            }

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

