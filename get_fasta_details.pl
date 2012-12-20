#!/usr/bin/perl

## Print sequence length, sequence number (in file), and FASTA header
## Can sort in ascending/descending order on sequence length
## Doesn't explicitly handle duplicates
## Last chages: 12/20/2012 10:18:13 AM
## Johan.Nylander\@abc.se

use strict;
use warnings;
use Data::Dumper;
use Getopt::Long;

my %seq_hash        = ();    # key:fasta header, val: seq
my %seq_length_hash = ();    # key:fasta header, val: seq
my %seq_nr_hash     = ();    # key:fasta header, val: seq_nr
my $term            = $/;
my $sort            = 0;
my $revsort         = 0;

my $res = GetOptions(
    'sort'    => \$sort,
    'revsort' => \$revsort,
    'help'    => sub { print "Usage: $0 [--sort] [--revsort] fastafile\n"; exit(0); },
);

die "\nTry $0 --help for more information\n\n" unless $res;

while ( my $fastafile = shift(@ARGV) ) {

    my $pos = 0;
    my $id;
    my @sequencelines;
    my $sequence;
    my $line;

    open( FASTA, "<", $fastafile ) or die("Open failed: $!");

    $/ = ">";
    while (<FASTA>) {
        chomp;
        # Since the file begins with ">", the first extraction will
        # contain only that '>', which will then get chomped, so we'll
        # have a blank line to skip.
        next if ( $_ eq '' );
        ( $id, @sequencelines ) = split /\n/;
        $seq_nr_hash{$id} = $pos;
        $sequence = '';
        foreach $line (@sequencelines) {
            $sequence .= $line;
        }
        $seq_length_hash{$id} = length($sequence);
        $seq_hash{$id}        = $sequence;
        $pos++;
    }
    $/ = $term;
}

#foreach my $key (sort { $seq_length_hash{$a} cmp $seq_length_hash{$b} } keys %seq_length_hash) {
#    print "$seq_length_hash{$key} = $key\n";
#}

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
    print "$seq_nr_hash{$key}\t$seq_length_hash{$key}\t$key\n";
}

