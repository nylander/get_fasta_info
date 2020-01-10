#!/usr/bin/env perl
#===============================================================================
=pod

=head2

         FILE: get_fasta_details.pl

        USAGE: ./get_fasta_details.pl [-s|-r] [-h] infile(s)  

  DESCRIPTION: Read FASTA-formatted file and report a tab-separated list of
               sequence length, sequence number (in file), file name, FASTA header
               
               Can sort in ascending/descending order on sequence length

      OPTIONS: -s, --sort    Sort the output on sequence length, shortest first
               -r, --revsort Sort the output on sequence length, longest first
               -h, --help    Display help text

 REQUIREMENTS: Uses perldoc for help documentation.

        NOTES: "sequence number (in file)" starts at 0.
        
               The fasta parser doesn't explicitly handle duplicate headers!

       AUTHOR: Johan Nylander (JN), johan.nylander@nrm.se

      COMPANY: NBIS/NRM

      VERSION: 2.0

      CREATED: < 2012

     REVISION: 2019-08-14 11:15:57

=cut


#===============================================================================

use strict;
use warnings;
use Getopt::Long;

my $sort    = 0;
my $revsort = 0;
my $term    = $/;

GetOptions(
    'sort'    => \$sort,
    'revsort' => \$revsort,
    'help'    => sub { exec("perldoc", $0); exit(0); },
);

die "Try $0 --help for more info.\n" unless @ARGV;

while ( my $fastafile = shift(@ARGV) ) {

    my %seq_hash        = ();
    my %seq_length_hash = ();
    my %seq_nr_hash     = ();

    my $pos = 0;
    my $id;
    my @sequencelines;
    my $sequence;
    my $line;

    open my $FASTA, "<", $fastafile or die("Open failed: $!");

    $/ = ">";
    while (<$FASTA>) {
        chomp;
        next if ($_ eq '');
        ($id, @sequencelines) = split /\n/;
        $seq_nr_hash{$id} = $pos;
        $sequence = '';
        foreach $line (@sequencelines) {
            $sequence .= $line;
        }
        $seq_length_hash{$id} = length($sequence);
        $seq_hash{$id} = $sequence;
        $pos++;
    }
    close($FASTA);
    $/ = $term;

    my @keys = ();
    if ($sort) {
        @keys = sort { length( $seq_hash{$a} ) <=> length( $seq_hash{$b} ) }
          keys %seq_hash;
    }
    elsif ($revsort) {
        @keys = sort { length( $seq_hash{$b} ) <=> length( $seq_hash{$a} ) }
          keys %seq_hash;
    }
    else {
        @keys = sort { $seq_nr_hash{$a} <=> $seq_nr_hash{$b} } keys %seq_hash;
    }
    foreach my $key (@keys) {
        print STDOUT "$seq_length_hash{$key}\t$seq_nr_hash{$key}\t$fastafile\t$key\t\n";
    }
}
