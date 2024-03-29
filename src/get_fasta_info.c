/*
*          File: get_fasta_info.c
*            By: Johan Nylander
* Last modified: fre apr 08, 2022  03:28
*   Description: Get min/max/avg sequence length in fasta.
*                Optionally, report min/max/avg missing data.
*                Mising data is any of the symbols 'NX?-' and
*                is reported as the fraction of missing data
*                in the sequence.
*                Can read compressed (gzip) files.
*                Prints to both stdout and stderr.
*       Compile: gcc -Wall -O3 -funroll-loops -o get_fasta_info get_fasta_info.c -lm -lz
*           Run: get_fasta_info fasta.fas
*       License: Copyright (c) 2019-2022 Johan Nylander
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

#define VERSION_STR "2.4.0"

int main (int argc, char **argv) {

    char *fname;
    char *res;
    char buf[PATH_MAX];
    char r; // r is the character currently read
    extern char *optarg;
    extern int optind;
    float avegap = 0.0f;
    float avelen = 0.0f;
    float fgap = 0.0f;
    float fgapsum;
    float maxgap;
    float mingap;
    int c, err = 0;
    int countgap = 0;
    int fullpath = 0;
    int inheader;
    int verbose = 1;
    long int lensum;
    long int maxlen;
    long int minlen;
    long int ngap;
    long int nseqs;
    long int seqlen;

    static char usage[] = "\nGet basic summary info about fasta formatted files.\n\nUsage:\n\n %s [-h][-n][-g][-p][-V] infile(s).\n\n  -h is help\n  -V is version\n  -n is noverbose\n  -g is count gaps (i.e. missing data symbols XN?-)\n  -p is to give full path to file\n\n  infile should be in fasta format.\n\n";

    if (argc == 1) {
        fprintf(stderr, usage, argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((c = getopt(argc, argv, "hVpng")) != -1) {
        switch (c) {
            case 'h':
                fprintf(stderr, usage, argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case 'V':
                fprintf(stdout, "%s\n", VERSION_STR);
                exit(EXIT_SUCCESS);
                break;
            case 'p':
                fullpath = 1;
                break;
            case 'n':
                verbose = 0;
                break;
            case 'g':
                countgap = 1;
                break;
            case '?':
                err = 1;
                break;
        }
    }

    if (err) {
        fprintf(stderr, usage, argv[0]);
        exit(EXIT_FAILURE);
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
            lensum = 0;
            ngap = 0;
            mingap = 1.0f;
            maxgap = 0.0f;
            fgapsum = 0.0f;

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
                            lensum += seqlen;
                        }
                        else {
                            minlen = 0;
                        }
                        if (countgap == 1) {
                            if (ngap > 0) {
                                fgap = ((float) ngap / (float) seqlen);
                                if (fgap > maxgap) {
                                    maxgap = fgap;
                                }
                                if (fgap < mingap) {
                                    mingap = fgap;
                                }
                                fgapsum = fgapsum + fgap;
                            }
                            else {
                                mingap = 0;
                            }
                            ngap = 0;
                        }
                        seqlen = 0;
                    }
                    ++nseqs;
                }
                else {
                    if (!isspace(r)) {
                        ++seqlen;
                    }
                    if (countgap == 1) {
                        if (r == '-' || r == 'X' || r == 'N' || r == '?') {
                            ++ngap;
                        }
                    }
                }
            }

            // Take care of last seq
            if (seqlen > maxlen) {
                maxlen = seqlen;
            }
            if (seqlen < minlen) {
                minlen = seqlen;
            }
            lensum += seqlen;

            if (countgap == 1) {
                fgap = ((float) ngap / (float) seqlen);
                if (fgap > maxgap) {
                    maxgap = fgap;
                }
                if (fgap < mingap) {
                    mingap = fgap;
                }
                fgapsum = fgapsum + fgap;
            }

            // If all empty sequences
            if (minlen == INT_MAX && maxlen == INT_MIN) {
                minlen = maxlen = 0;
            }

            if (lensum > 0) {
                avelen = 1.0 * lensum / nseqs;
            }
            else {
                avelen = 0.0;
            }

            if (countgap == 1) {
                if (fgapsum > 0.0) {
                    avegap = fgapsum / nseqs;
                }
                else {
                    avegap = 0.0;
                }
            }

            if (verbose) {
                if (countgap == 1) {
                    fprintf(stderr, "%s", "Nseqs\tMin.len\tMax.len\tAvg.len\tMin.gap\tMax.gap\tAvg.gap\tFile\n");
                }
                else {
                    fprintf(stderr, "%s", "Nseqs\tMin.len\tMax.len\tAvg.len\tFile\n");
                }
            }

            if (countgap == 1) {
                fprintf(stdout, "%ld\t%ld\t%ld\t%g\t%.2f\t%.2f\t%.2f\t", nseqs, minlen, maxlen, round(avelen), mingap, maxgap, avegap);
            }
            else {
                fprintf(stdout, "%ld\t%ld\t%ld\t%g\t", nseqs, minlen, maxlen, round(avelen));
            }

            if (fullpath == 1) {
                res = realpath(fname, buf);
                if (res) {
                    fprintf(stdout, "%s\n", buf);
                }
                else {
                    fprintf(stdout, "Error: Failed getting realpath of infile %s.\n", fname);
                    exit(EXIT_FAILURE);
                }
            }
            else {
                fprintf(stdout,"%s\n", basename(fname));
            }

            minlen = INT_MAX;
            maxlen = INT_MIN;
            nseqs = 0;
            seqlen = 0;
            lensum = 0;

            if (countgap == 1) {
                mingap = 1.0f;
                maxgap = 0.0f;
                ngap = 0;
            }

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

