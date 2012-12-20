#!/usr/bin/perl

## Usage: get_fasta_info.gz.pl infile.fa
## Internet source modified by JN.
## Last changes: 12/20/2012 10:03:43 AM

use strict;
use Data::Dumper;

my (
    $my_name,      $Total_Contigs, $Total_XContigs, $Total_Bases,
    $Total_XBases, $Total_Min,     $Total_Max,      $Unique_Counts
);

#===  FUNCTION  ================================================================
#         NAME:  read_file
#      VERSION:  02/09/2012 12:11:04 PM
#  DESCRIPTION:  ???
#   PARAMETERS:  ???
#      RETURNS:  ???
#         TODO:  ???
#===============================================================================
sub read_file {
    my ($file_name) = @_;

    ## open file
    ## check if compressed. Warning, does not handle tar archives (*.tar.gz, *.tar.bz2, *.tgz, etc)
    if ( $file_name =~ /\.gz$/ ) {
        $file_name =~ s/(.*\.gz)\s*$/gzip -dc < $1|/;
    }
    elsif ( $file_name =~ /\.zip$/ ) {
        $file_name =~ s/(.*\.zip)\s*$/gzip -dc < $1|/;
    }
    elsif ( $file_name =~ /\.Z$/ ) {
        $file_name =~ s/(.*\.Z)\s*$/gzip -dc < $1|/;
    }
    elsif ( $file_name =~ /\.bz2$/ ) {
        $file_name =~ s/(.*\.bz2)\s*$/bzip2 -dc < $1|/;
    }
    open( FASTAIN, $file_name ) or die "Can't open $file_name : $!";

    ## read contigs in file
    my ( $header, $contig, $sequence, $line ) = ('') x 4;
    my ( $contigs, $xcontigs, $bases, $xbases, $max, $ave, $line_num ) =
      (0) x 7;
    my $min = undef;
    while ( defined( $line = <FASTAIN> ) ) {
        chomp $line;
        $line_num++;
        if ( $line =~ /^>/ ) {
            if ($contig) {   # after first input line?
                ( $contigs, $xcontigs, $bases, $xbases, $min, $max ) =
                  process_contig( $contig, $sequence, $contigs, $xcontigs,
                    $bases, $xbases, $min, $max );
            }
            $header = $line;
            if ( $header =~ m/^>(\S+)/ ) {
                $contig = $1;
            }
            else {
                die "$my_name: Invalid fasta file header at line $line_num\n"
                  . "of file: '$file_name'\n";
            }
            $sequence = '';
            next;
        }    # end if ($line =~ /^>/)
        if ( !$contig ) {
            die "$my_name: File '$file_name' does not begin with a fasta header line\n";
        }
        $line =~ s/\s//g;
        $sequence .= $line;
    }    # end while (defined($line = <FASTAIN>))
    close(FASTAIN);
    if ($contig) {
        ( $contigs, $xcontigs, $bases, $xbases, $min, $max ) =
          process_contig( $contig, $sequence, $contigs, $xcontigs, $bases,
            $xbases, $min, $max );
    }

    ## print stats and accumulate totals
    $ave = $contigs ? ( sprintf "%.1f", $bases / $contigs ) : 0;
    print_counts( $contigs, $xcontigs, $bases, $xbases, $min, $max, $ave,
        $file_name );
    $Total_Contigs  += $contigs;
    $Total_XContigs += $xcontigs;
    $Total_Bases    += $bases;
    $Total_XBases   += $xbases;
    $Total_Min = $min
      if ( ( !defined $Total_Min ) || ( $Total_Min > $min ) );
    $Total_Max = $max if ( $Total_Max < $max );

    #print STDERR "Bases: ", $Total_Bases, "\n";

    return;

}    # end read_file


#===  FUNCTION  ================================================================
#         NAME:  process_contig
#      VERSION:  03/11/2009 03:03:13 PM PST
#  DESCRIPTION:  ???
#   PARAMETERS:  ???
#      RETURNS:  ???
#         TODO:  ???
#===============================================================================
sub process_contig {
    my ( $contig, $sequence, $contigs, $xcontigs, $bases, $xbases, $min, $max )
      = @_;
    my $len = length $sequence;
    $contigs++;
    $bases += $len;
    if ($Unique_Counts) {
        $xcontigs++;
        $xbases += $len;
        if ( $contig =~ /__(\d+)$/ ) {
            my $extra = $1;
            $xcontigs += $extra;
            $xbases   += $len * $extra;
        }
    }
    $min = $len if ( ( !defined $min ) || ( $min > $len ) );
    $max = $len if ( $max < $len );

    return ( $contigs, $xcontigs, $bases, $xbases, $min, $max );

}    # end process_contig


#===  FUNCTION  ================================================================
#         NAME:  print_counts
#      VERSION:  03/11/2009 03:03:30 PM PST
#  DESCRIPTION:  ???
#   PARAMETERS:  ???
#      RETURNS:  ???
#         TODO:  ???
#===============================================================================
sub print_counts {
    my ( $contigs, $xcontigs, $bases, $xbases, $min, $max, $ave, $file_name ) =
      @_;

    if ($Unique_Counts) {
        printf STDOUT "%7d %7d %13d %13d %7d %7d %9.1f  %s\n",
          $contigs, $xcontigs, $bases, $xbases, $min, $max, $ave,
          $file_name;
    }
    else {
        printf STDOUT
            "\n# File: %s\nNseqs: %9d\nMin. length: %d\nMax. length: %d\nAvg. length: %d\n",
            $file_name, $contigs, $min, $max, $ave;
    }

    return;

}    # end print_counts


#===  FUNCTION  ================================================================
#         NAME:  "MAIN"
#      VERSION:  03/11/2009 03:08:14 PM PST
#  DESCRIPTION:  ???
#   PARAMETERS:  ???
#      RETURNS:  ???
#         TODO:  ???
#===============================================================================
MAIN:
while ( my $infile = shift(@ARGV) ) {
    read_file($infile);
}
exit(0);

__END__
