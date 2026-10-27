#!/usr/bin/env perl
#===============================================================================

=pod

=head2

         FILE: get_fasta_details.pl

        USAGE: ./get_fasta_details.pl [-s|-r] [-h,-v] infile(s)

  DESCRIPTION: Read FASTA-formatted file and report a tab-separated list of
               sequence length, sequence number (in file), file name, FASTA header

               Can sort in ascending/descending order on sequence length.
               Can read compressed (.gz) files.

      OPTIONS: -s, --sort    Sort the output on sequence length, shortest first
               -r, --revsort Sort the output on sequence length, longest first
               -v, --version Print version
               -h, --help    Display help text

 REQUIREMENTS: Uses core perl modules.

        NOTES: "sequence number (in file)" starts at 0.
               The fasta parser doesn't explicitly handle duplicate headers!

       AUTHOR: Johan Nylander (JN), johan.nylander@nrm.se

      COMPANY: NRM

      VERSION: 2.1

      CREATED: < 2012

     REVISION: 2026-10-27

      LICENSE: Copyright (c) 2019-2026 Johan Nylander

               Permission is hereby granted, free of charge, to any person
               obtaining a copy of this software and associated documentation
               files (the "Software"), to deal in the Software without
               restriction, including without limitation the rights to use,
               copy, modify, merge, publish, distribute, sublicense, and/or
               sell copies of the Software, and to permit persons to whom the
               Software is furnished to do so, subject to the following
               conditions:

               The above copyright notice and this permission notice shall be
               included in all copies or substantial portions of the Software.

               THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
               EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
               OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
               NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
               HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
               WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
               FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
               OTHER DEALINGS IN THE SOFTWARE.

=cut

#===============================================================================

use strict;
use warnings;
use Getopt::Long;
use Pod::Usage             qw(pod2usage);
use IO::Uncompress::Gunzip qw(gunzip $GunzipError);

my $version = "2.1";
my $sort    = 0;
my $revsort = 0;

GetOptions(
    'sort'    => \$sort,
    'revsort' => \$revsort,
    'version' => sub { print $version, "\n"; exit(0); },
    "help"    => sub {
        pod2usage(
            -verbose   => 2,
            -noperldoc => 1,
            -exitval   => 0,
            -output    => \*STDOUT,
        );
    },
);

die "Try $0 --help for more info.\n" unless @ARGV;

while (my $fastafile = shift(@ARGV)) {

    my %seq_length_hash = ();
    my %seq_nr_hash     = ();

    my $pos = 0;
    my $id;

    my $FASTA;

    if ($fastafile =~ /\.gz$/) {
        $FASTA = IO::Uncompress::Gunzip->new($fastafile)
          or die "Cannot open gz file '$fastafile': $GunzipError\n";
    }
    else {
        open $FASTA, '<', $fastafile
          or die "Cannot open file '$fastafile': $!\n";
    }

    while (my $line = <$FASTA>) {
        chomp $line;
        $line =~ s/\r\z//; # handle CRLF input

        if ($line =~ /^>(.*)\z/) {
            $id                   = $1;
            $seq_nr_hash{$id}     = $pos++;
            $seq_length_hash{$id} = 0;
        }
        elsif (defined $id) {
            $seq_length_hash{$id} += length($line);
        }
    }

    close($FASTA);

    my @keys = ();

    if ($sort) {
        @keys = sort {
                 $seq_length_hash{$a} <=> $seq_length_hash{$b}
              || $seq_nr_hash{$a} <=> $seq_nr_hash{$b}
        } keys %seq_length_hash;
    }
    elsif ($revsort) {
        @keys = sort {
                 $seq_length_hash{$b} <=> $seq_length_hash{$a}
              || $seq_nr_hash{$a} <=> $seq_nr_hash{$b}
        } keys %seq_length_hash;
    }
    else {
        @keys = sort { $seq_nr_hash{$a} <=> $seq_nr_hash{$b} }
          keys %seq_length_hash;
    }

    foreach my $key (@keys) {
        print STDOUT
          "$seq_length_hash{$key}\t$seq_nr_hash{$key}\t$fastafile\t$key\t\n";
    }
}
