/*
*          File: get_fastq_info.c
*            By: Johan Nylander
* Last modified: ons feb 26, 2025  05:44
*   Description: Get min/max/avg sequence length in fastq.
*                Can read compressed (gzip) files.
*                Prints to both stdout and stderr.
*                Can report average read quality (phred, ASCII_BASE=33) score.
*       Compile: gcc -Wall -O3 -funroll-loops -o get_fastq_info get_fastq_info.c -lm -lz
*           Run: get_fastq_info -q fastq.fq.gz
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
#include <zlib.h>

#define VERSION_STR "2.4.1"

void print_usage(const char *prog_name) {
    fprintf(stderr, "\n%s v%s\n", prog_name, VERSION_STR);
    fprintf(stderr, "\nGet basic summary info about fastq formatted files.\n\n");
    fprintf(stderr, "Usage:\n\n %s [options] infile(s).\n\n", prog_name);
    fprintf(stderr, "Options:\n\n");
    fprintf(stderr, " -h  help\n");
    fprintf(stderr, " -V  version\n");
    fprintf(stderr, " -n  noverbose\n");
    fprintf(stderr, " -q  show avg. read qual (ASCII_BASE=33)\n");
    fprintf(stderr, " infile should be in fastq format (gzipped or not).\n\n");
}

void process_file(const char *fname, int verbose, int quality, int fullpath) {
    char buf[PATH_MAX];
    char r; // r is the character currently read
    gzFile zfp = gzopen(fname, "rb");
    if (zfp == NULL) {
        perror("Error: failed in opening file");
        exit(EXIT_FAILURE);
    }

    long int minlen = INT_MAX;
    long int maxlen = INT_MIN;
    long int nseqs = 0;
    long int seqlen = 0;
    long int sum = 0;
    long int qsum = 0;
    int linecounter = 0;
    int sqsum = 0;

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
                if (quality && sqsum > 0) {
                    qsum += round(sqsum / (float)seqlen);
                    sqsum = 0;
                }
                ++nseqs;
                linecounter = 0;
                seqlen = 0;
            }
        }
        else if (linecounter == 1 && !isspace(r)) {
            ++seqlen;
        }
        else if (linecounter == 3 && quality && !isspace(r)) {
            sqsum += r - 33; // ASCII_BASE=33
        }
    }

    float avelen = 1.0 * sum / nseqs;
    float avequal = quality ? 1.0 * qsum / nseqs : 0;

    if (verbose) {
        if (quality) {
            fprintf(stderr, "Nseqs\tMin.len\tMax.len\tAvg.len\tAvg.qual\tFile\n");
        }
        else {
            fprintf(stderr, "Nseqs\tMin.len\tMax.len\tAvg.len\tFile\n");
        }
    }

    if (quality) {
        fprintf(stdout, "%ld\t%ld\t%ld\t%g\t%g\t", nseqs, minlen, maxlen, round(avelen), round(avequal));
    }
    else {
        fprintf(stdout, "%ld\t%ld\t%ld\t%g\t", nseqs, minlen, maxlen, round(avelen));
    }

    if (fullpath) {
        char *res = realpath(fname, buf);
        if (res) {
            fprintf(stdout, "%s\n", buf);
        }
        else {
            fprintf(stdout, "Error: Failed getting realpath of infile %s.\n", fname);
            exit(EXIT_FAILURE);
        }
    }
    else {
        fprintf(stdout, "%s\n", basename((char*)fname));
    }

    gzclose(zfp);
}

int main(int argc, char **argv) {
    int c;
    int err = 0;
    int fullpath = 0;
    int verbose = 1;
    int quality = 0;

    if (argc == 1) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((c = getopt(argc, argv, "hVpnq")) != -1) {
        switch (c) {
            case 'h':
                print_usage(argv[0]);
                exit(EXIT_SUCCESS);
            case 'V':
                fprintf(stdout, "%s\n", VERSION_STR);
                exit(EXIT_SUCCESS);
            case 'p':
                fullpath = 1;
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
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (optind < argc) {
        for (; optind < argc; optind++) {
            process_file(argv[optind], verbose, quality, fullpath);
        }
    } else {
        fprintf(stderr, "Error: Need input fastq file(s) to process.\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
