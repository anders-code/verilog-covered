# Name:     err5.2.pl
# Author:   Trevor Williams  (phase1geo@gmail.com)
# Date:     06/21/2008
# Purpose:  Remove data/coverage portion of vector information in CDD for a signal and verify
#           that when this information is merged in that we get a parsing error.
# Note:     If the format of the vsignal or vector structures change, this diagnostic will most likely
#           fail and will need to be updated accordingly.

require "../verilog/regress_subs.pl";

# Initialize the diagnostic environment
&initialize( "err5.2", 1, @ARGV );

# Simulate and get coverage information
system( "make DIAG=err5.2a oneerrmergerun" ) && die;
system( "make DIAG=err5.2b oneerrmergerun" ) && die;
system( "mv err5.2b.cdd err5.2b.tmp.cdd" ) && die;

# Modify the version to something which is different
open( OLD_CDD, "err5.2b.tmp.cdd" ) || die "Can't open err5.2b.tmp.cdd for reading: $!\n";
open( NEW_CDD, ">err5.2b.cdd" ) || die "Can't open err5.2b.cdd for writing: $!\n";
while( <OLD_CDD> ) {
  chomp;
  @line = split;
  if( ($line[0] eq "1") && ($line[1] eq "a") ) {
    $#line = 9;
    # vsignal outputs 6 items + 2 dimensional items, vector outputs 2 items + data items -- so truncate line to 8 items
    print NEW_CDD "@line\n";
  } else {
    print NEW_CDD "$_\n";
  }
}
close( NEW_CDD );
close( OLD_CDD );
system( "rm -f err5.2b.tmp.cdd" ) && die;

# Attempt to run the score command to score it
&runMergeCommand( "err5.2a.cdd err5.2b.cdd 2> err5.2.err" );
system( "cat err5.2.err" ) && die;

# Perform the file comparison checks
&checkTest( "err5.2", 3, 1 );

exit 0;

