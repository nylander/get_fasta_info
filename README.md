# Get FASTA info

## Description

Some useful summary tools for FASTA formatted files.

These scripts gives a brief summary of min, average, max seq lengths
for FASTA formatted sequence files.

#### Note

The `get_fasta_info.pl` is based on modified code from the internet.
I will acknowledge the original author (as soon as I locate him/her!).
If you recognize the code, please send me a note!


## Scripts

### `get_fasta_info.pl`

##### Usage

    ./get_fasta_info.pl [-n] [-h] infile(s) 

##### Description

Gives a brief summary of min, average, and max seq lengths for FASTA formatted
sequence files.

The script can also read compressed files (.gz, .zip, .Z, .bz2). It does
not, however, handle compressed tar archives (.tar.gz, etc).

Output fields are tab separated and headers are printed to stdout and stderr
for easy parsing.

##### Options

- `-h`, `--help`      Show help text
- `-n`, `--noverbose` Do not print the header (Nseqs Min.len Max.len Avg.len File)

##### Examples

    $ ./get_fasta_info.pl dat/*.fas
    Nseqs Min.len Max.len Avg.len File
    9     643     649     647     1.fas
    Nseqs Min.len Max.len Avg.len File
    14    216     339     289     2.fas
    
    $ ./get_fasta_info.pl -n dat/*.fas
    9      643     649     647     dat/1.fas
    14     216     339     289     dat/2.fas
    
    $ ./get_fasta_info.pl dat/*.fas 2>/dev/null | \
        sort -k1 | awk '{print $NF,$1}'
    2.fas 14
    1.fas 9


### `get_fasta_details.pl`

##### Usage

    ./get_fasta_details.pl [-s|-r] [-h] infile(s)  

##### Description

Read FASTA-formatted file and report a tab-separated list of sequence length,
sequence number (in file), file name, FASTA header
                  
Can sort in ascending/descending order on sequence length.

##### Options

- `-s`, `--sort`    Sort the output on sequence length, shortest first
- `-r`, `--revsort` Sort the output on sequence length, longest first
- `-h`, `--help`    Display help text

##### Examples

    $ ./get_fasta_details.pl dat/*.fas
    649	0	dat/1.fas	gi|256009056|gb|ACU54623.1| ...	
    643	1	dat/1.fas	gi|586972334|gb|EWT07678.1| ...	
    645	2	dat/1.fas	gi|908398554|ref|WP_049755449.1| ...
    ...
    318	0	dat/2.fas	gi|949018528|gb|KRO28616.1| ...
    283	1	dat/2.fas	gi|949023402|gb|KRO32148.1| ...
    315	2	dat/2.fas	gi|949028303|gb|KRO35658.1| ...

