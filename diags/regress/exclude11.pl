# Name:     exclude11.pl
# Author:   Trevor Williams  (phase1geo@gmail.com)
# Date:     09/17/2008
# Purpose:  

require "../verilog/regress_subs.pl";

# Initialize the diagnostic environment
&initialize( "exclude11", 0, @ARGV );

# Perform generate
&runGenerateCommand( "-t main -v exclude11.v" );

# Simulate and get coverage information
if( $SIMULATOR eq "IV" ) {
  system( "iverilog -DDUMP covered/verilog/exclude11.v; ./a.out" ) && die;
} elsif( $SIMULATOR eq "CVER" ) {
  system( "cver -q +define+DUMP covered/verilog/exclude11.v" ) && die;
} elsif( $SIMULATOR eq "VCS" ) {
  system( "vcs +define+DUMP covered/verilog/exclude11.v; ./simv" ) && die;
} elsif( $SIMULATOR eq "VERIWELL" ) {
  system( "veriwell +define+DUMP covered/verilog/exclude11.v" ) && die;
}

# Perform diagnostic running code here
&runScoreCommand( "-vcd exclude11.vcd exclude11" );

# Create temporary file that will contain an exclusion message
&runCommand( "echo This logic is not needed > exclude11a.excl" );
&runCommand( "echo This logic is also not needed > exclude11b.excl" );
&runCommand( "echo This logic is additionally not needed > exclude11c.excl" );

# Perform exclusion
&runExcludeCommand( "-m L07 < exclude11a.excl" );
&runExcludeCommand( "-m L10 < exclude11b.excl" );
&runExcludeCommand( "-m L13 < exclude11c.excl" );
&runExcludeCommand( "L07" );
&runExcludeCommand( "L13" );

# Remove temporary exclusion reason file
system( "rm -f exclude11a.excl exclude11b.excl exclude11c.excl" ) && die;

# Generate reports
&runReportCommand( "-d v -e -x -o exclude11.rptM exclude11" );
&runReportCommand( "-d v -e -x -i -o exclude11.rptI exclude11" );

# Perform the file comparison checks
&checkTest( "exclude11", 1, 0 );

exit 0;

