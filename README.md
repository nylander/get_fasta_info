get_fasta_info
==============

Some useful summary tools for FASTA formatted files.

These scripts gives a brief summary of min, average, max seq lengths
for FASTA formatted sequence files.

The script can also read compressed files (`.gz`, `.zip`, `.Z`, `.bz2`).

Fields are tab separated and headers are printed to stderr for easy parsing
of output.

Examples:

    $ get_fasta_info.pl *.fas
    Nseqs Min.len Max.len Avg.len File
    9     643     649     647     1.fas
    Nseqs Min.len Max.len Avg.len File
    14    216     339     289     2.fas

    $ ./get_fasta_info.pl *.fas  2>/dev/null | sort -k1 | awk '{print $NF,$1}'
    2.fas 14
    1.fas 9



The `get_fasta_info.pl` is based on modified code from the internet.
I will acknowledge the original author (as soon as I locate him/her!).
If you recognize the code, please send me a note!


