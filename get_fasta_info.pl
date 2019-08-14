#!/usr/bin/env perl
#===============================================================================

=pod

=head2

         FILE: get_fasta_info.pl

        USAGE: ./get_fasta_info.pl [-n] [-h] infile(s) 

  DESCRIPTION: Gives a brief summary of min, average, and
               max seq lengths for FASTA formatted sequence files.

               The script can also read compressed files
               (.gz, .zip, .Z, .bz2). Note, it does not handle
               compressed tar archives (.tar.gz, etc).

               Output fields are tab separated and headers are
               printed to stdout and stderr for easy parsing.

               Examples:

                 $ ./get_fasta_info.pl dat/*.fas
                 Nseqs Min.len Max.len Avg.len File
                 9     643     649     647     1.fas
                 Nseqs Min.len Max.len Avg.len File
                 14    216     339     289     2.fas

                 $ ./get_fasta_info.pl -n dat/*.fas
                 9	643	649	647	dat/1.fas
                 14	216	339	289	dat/2.fas

                 $ ./get_fasta_info.pl dat/*.fas 2>/dev/null | \
                     sort -k1 | awk '{print $NF,$1}'
                 2.fas 14
                 1.fas 9

      OPTIONS: -h,--help        Show help text
               -n, --noverbose  Do not print the header
                                (Nseqs Min.len Max.len Avg.len File)

 REQUIREMENTS: Uses perldoc for documentation.
               Uses gzip and bzip2 on compressed files.

         BUGS: ---

        NOTES: The script does not handle compressed tar archives.
        
               The `get_fasta_info.pl` is based on modified code
               from the internet. I will acknowledge the original
               author (as soon as I locate him/her!).
               If you recognize the code, please send me a note!

       AUTHOR: Johan Nylander (JN), johan.nylander@nrm.se

      COMPANY: NBIS/NRM

      VERSION: 2.0

      CREATED: < 2009 

     REVISION: 2019-08-14 09:36:22

=cut

#===============================================================================

use strict;
use warnings;
use Getopt::Long;


## main
my $VERBOSE = 1;

GetOptions(
    "verbose!" => \$VERBOSE,
    "help"     => sub {exec("perldoc", $0); exit(0);},
);

while (my $infile = shift(@ARGV)) {
    read_file($infile);
}

exit(0);


## Functions
sub read_file {

    my ($file_name) = @_;

    my $header   = q{};
    my $contig   = q{};
    my $sequence = q{};
    my $line     = q{};
    my $contigs  = 0;
    my $bases    = 0;
    my $max      = 0;
    my $ave      = 0;
    my $line_num = 0;
    my $min      = undef;

    if ($file_name =~ /\.gz$/) {
        $file_name =~ s/(.*\.gz)\s*$/gzip -dc < $1|/;
    }
    elsif ($file_name =~ /\.zip$/) {
        $file_name =~ s/(.*\.zip)\s*$/gzip -dc < $1|/;
    }
    elsif ($file_name =~ /\.Z$/) {
        $file_name =~ s/(.*\.Z)\s*$/gzip -dc < $1|/;
    }
    elsif ($file_name =~ /\.bz2$/) {
        $file_name =~ s/(.*\.bz2)\s*$/bzip2 -dc < $1|/;
    }

    open (FASTAIN, $file_name) or die "Can't open $file_name : $!";

    while (defined($line = <FASTAIN>)) {
        chomp $line;
        $line_num++;
        if ($line =~ /^\s*>/) {
            if ($contig) {
                ($contigs, $bases, $min, $max) = process_contig($contig, $sequence, $contigs, $bases, $min, $max);
            }
            $header = $line;
            if ($header =~ m/^\s*>\s*(\S+.*)$/) {
                $contig = $1;
            }
            else {
                die "Error: Could not read fasta file header at line $line_num\n"
                  . "in file: '$file_name'\n";
            }
            $sequence = '';
            next;
        }

        if (!$contig) {
            die "Error: Does '$file_name' begin with a fasta header?\n";
        }

        $line =~ s/\s//g;
        $sequence .= $line;
    }

    close(FASTAIN);

    if ($contig) {
        ($contigs, $bases, $min, $max) = process_contig($contig, $sequence, $contigs, $bases, $min, $max);
    }

    $ave = $contigs ? (sprintf "%.1f", $bases/$contigs) : 0;

    print_counts($contigs, $bases, $min, $max, $ave, $file_name);

    return;

}


sub process_contig {

    my ($contig, $sequence, $contigs, $bases, $min, $max) = @_;

    my $len = length $sequence;

    $contigs++;
    $bases += $len;

    if ((!defined $min) || ($min > $len)) {
        $min = $len;
    }

    if ($max < $len) {
        $max = $len;
    }

    return ($contigs, $bases, $min, $max);

}


sub print_counts {

    my ($contigs, $bases, $min, $max, $ave, $file_name) = @_;

    if ($VERBOSE) {
        print STDERR "Nseqs\tMin.len\tMax.len\tAvg.len\tFile\n";
    }

    printf STDOUT "%d\t%d\t%d\t%d\t%s\n", $contigs, $min, $max, $ave, $file_name;

    return;

}


__END__
