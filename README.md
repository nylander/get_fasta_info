# Get FASTA info

Some useful summary tools for FASTA formatted files.

These tools gives a brief summary of number of sequences, min, max, and average
sequence lengths, for FASTA formatted files.


## `get_fasta_info`

##### Description:

Program written in C. Will report number of sequences, min/max/average sequence
lengths, and file name read, as tab-delimited output. Prints to both stdout and
stderr. Can read compressed (gzip) input files.

Note: If empty sequences are present, their length (0) will still be used when
calculating the average sequence length in the file.

##### To compile:

    cd src
    make
    make test

##### Options:

- `-h` prints brief Usage information

##### Examples:

    $ src/get_fasta_info dat/fasta.*
    Nseqs	Min.len	Max.len	Avg.len	File
    3	1	3	2	fasta.fasta
    Nseqs	Min.len	Max.len	Avg.len	File
    3	1	3	2	fasta.fasta.gz

    $ src/get_fasta_info dat/fasta.* 2>/dev/null
    3	1	3	2	fasta.fasta
    3	1	3	2	fasta.fasta.gz


## `get_fasta_info.pl`

##### Description:

Perl-script with the same basic functionality as the (faster) C-program, but
can also read `bzip` format (if `bzip2` is installed) in addition to `.gz`,
`.zip`, and `.Z`. It does not, however, handle compressed tar archives
(`.tar.gz`, etc).

##### Options:

- `-h` prints brief Usage information
- `-n` do not print the header

##### Examples:

    $ scripts/get_fasta_info.pl dat/*.fas
    Nseqs Min.len Max.len Avg.len File
    9     643     649     647     1.fas
    Nseqs Min.len Max.len Avg.len File
    14    216     339     290     2.fas
    
    $ scripts/get_fasta_info.pl -n dat/*.fas
    9	643	649	647	dat/1.fas
    14	216	339	290	dat/2.fas
    
    $ scripts/get_fasta_info.pl dat/*.fas 2>/dev/null | \
        sort -k4 | awk '{print $NF,$1}'
    dat/2.fas 14
    dat/1.fas 9

*Note:* The `get_fasta_info.pl` is based on modified code from the internet. I
will acknowledge the original author (as soon as I locate him/her!). If you
recognize the code, please send me a note!


## `get_fasta_details.pl`

##### Description:

Perl-script to read FASTA-formatted file and report a tab-separated list of
sequence length, sequence number (in file), file name, FASTA header
                  
Can sort in ascending/descending order on sequence length.

##### Options:

- `-s`, `--sort`    Sort the output on sequence length, shortest first
- `-r`, `--revsort` Sort the output on sequence length, longest first
- `-h`, `--help`    Display help text

##### Examples:

    $ scripts/get_fasta_details.pl dat/*.fas
    649	0	dat/1.fas	gi|256009056|gb|ACU54623.1| ...	
    643	1	dat/1.fas	gi|586972334|gb|EWT07678.1| ...	
    645	2	dat/1.fas	gi|908398554|ref|WP_049755449.1| ...
    ...
    318	0	dat/2.fas	gi|949018528|gb|KRO28616.1| ...
    283	1	dat/2.fas	gi|949023402|gb|KRO32148.1| ...
    315	2	dat/2.fas	gi|949028303|gb|KRO35658.1| ...
    ...
