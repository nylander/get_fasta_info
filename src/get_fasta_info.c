/*
*          File: get_fasta_info.c
*            By: Johan Nylander
* Last modified: ons feb 26, 2025  05:06
*   Description: Get min/max/avg sequence length in fasta.
*                Optionally, report min/max/avg missing data.
*                Mising data is any of the symbols 'Nn?Xx-' (can be
*                changed by options) and is reported as the fraction
*                of missing data in the sequence.
*                Can read compressed (gzip) files.
*                Prints to both stdout and stderr.
*       Compile: gcc -Wall -O3 -funroll-loops -o get_fasta_info get_fasta_info.c -lm -lz
*           Run: get_fasta_info fasta.fas
*       License: Copyright (c) 2019-2025 Johan Nylander
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
#include <string.h>
#include <zlib.h>

#define VERSION_STR "2.4.1"
#define DEFAULT_MISSING_CHARS "Nn?Xx-"

void print_usage(char *prog_name) {
    fprintf(stderr, "\n%s v%s\n", prog_name, VERSION_STR);
    fprintf(stderr, "\nGet basic summary info about fasta formatted files.\n\n");
    fprintf(stderr, "Usage:\n\n %s [options] infile(s).\n\n", prog_name);
    fprintf(stderr, "Options:\n\n");
    fprintf(stderr, " -h        help\n");
    fprintf(stderr, " -V        version\n");
    fprintf(stderr, " -n        noverbose\n");
    fprintf(stderr, " -g        count gaps, i.e. missing data symbols. Default: %s\n", DEFAULT_MISSING_CHARS);
    fprintf(stderr, " -C chars  use char(s) as missing symbols and use -g\n");
    fprintf(stderr, " -N        -C N -g\n");
    fprintf(stderr, " -X        -C X -g\n");
    fprintf(stderr, " -Q        -C ? -g\n");
    fprintf(stderr, " -G        -C - -g\n");
    fprintf(stderr, " -p        print full path to file\n\n");
    fprintf(stderr, " infile should be in fasta format.\n\n");
}

void print_version() {
    fprintf(stdout, "%s\n", VERSION_STR);
}

void process_file(char *fname, int countgap, char *missing_chars, int fullpath, int verbose) {
    gzFile zfp = gzopen(fname, "rb");
    if (zfp == NULL) {
        perror("Error: failed in opening file");
        exit(EXIT_FAILURE);
    }

    char buf[PATH_MAX];
    char r;
    char *res;
    int inheader = 0;
    long int nseqs = 0, seqlen = 0;
    long int lensum = 0, ngap = 0;
    long int minlen = INT_MAX, maxlen = INT_MIN;
    float mingap = 1.0f, maxgap = 0.0f, fgapsum = 0.0f;
    float avegap = 0.0f, avelen = 0.0f;

    while ((r = gzgetc(zfp)) != EOF) {
        if (inheader) {
            if (r == '\n') inheader = 0;
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
                if (countgap) {
                    if (ngap > 0) {
                        float fgap = (float) ngap / seqlen;
                        if (fgap > maxgap) {
                            maxgap = fgap;
                        }
                        if (fgap < mingap) {
                            mingap = fgap;
                        }
                        fgapsum += fgap;
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
            if (countgap && strchr(missing_chars, r)) {
                ++ngap;
            }
        }
    }

    if (seqlen > maxlen) {
        maxlen = seqlen;
    }
    if (seqlen < minlen) {
        minlen = seqlen;
    }
    lensum += seqlen;

    if (countgap) {
        float fgap = (float) ngap / seqlen;
        if (fgap > maxgap) {
            maxgap = fgap;
        }
        if (fgap < mingap) {
            mingap = fgap;
        }
        fgapsum += fgap;
    }

    if (minlen == INT_MAX && maxlen == INT_MIN) {
        minlen = maxlen = 0;
    }
    avelen = lensum > 0 ? (float) lensum / nseqs : 0.0f;
    if (countgap) {
        avegap = fgapsum > 0.0 ? fgapsum / nseqs : 0.0;
    }

    if (verbose) {
        fprintf(stderr, "%s", countgap ? "Nseqs\tMin.len\tMax.len\tAvg.len\tMin.gap\tMax.gap\tAvg.gap\tFile\n" : "Nseqs\tMin.len\tMax.len\tAvg.len\tFile\n");
    }

    if (countgap) {
        fprintf(stdout, "%ld\t%ld\t%ld\t%ld\t%.2f\t%.2f\t%.2f\t", nseqs, minlen, maxlen, lround(avelen), mingap, maxgap, avegap);
    }
    else {
        fprintf(stdout, "%ld\t%ld\t%ld\t%ld\t", nseqs, minlen, maxlen, lround(avelen));
    }

    if (fullpath) {
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
        fprintf(stdout, "%s\n", basename(fname));
    }

    gzclose(zfp);
}

int main(int argc, char **argv) {
    int c, err = 0;
    int countgap = 0, fullpath = 0, verbose = 1;
    char *missing_chars = DEFAULT_MISSING_CHARS;

    while ((c = getopt(argc, argv, "hVngNXQGC:p")) != -1) {
        switch (c) {
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            case 'V':
                print_version();
                exit(EXIT_SUCCESS);
            case 'n':
                verbose = 0;
                break;
            case 'g':
                countgap = 1;
                break;
            case 'N':
                countgap = 1;
                missing_chars = "N";
                break;
            case 'X':
                countgap = 1;
                missing_chars = "X";
                break;
            case 'Q':
                countgap = 1;
                missing_chars = "?";
                break;
            case 'G':
                countgap = 1;
                missing_chars = "-";
                break;
            case 'C':
                countgap = 1;
                missing_chars = optarg;
                break;
            case 'p':
                fullpath = 1;
                break;
            case '?':
                err = 1;
                break;
        }
    }

    if (err) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (optind >= argc) {
        fprintf(stderr, "Error: Need input fasta file(s) to process.\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    for (; optind < argc; optind++) {
        process_file(argv[optind], countgap, missing_chars, fullpath, verbose);
    }

    return EXIT_SUCCESS;
}
