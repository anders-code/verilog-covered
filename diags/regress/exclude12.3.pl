# Name:     exclude12.3.pl
# Author:   Trevor Williams  (phase1geo@gmail.com)
# Date:     09/17/2008
# Purpose:  Verify that an excluded combinational coverage point can be output correctly.

require "../verilog/regress_subs.pl";

# Initialize the diagnostic environment
&initialize( "exclude12.3", 1, @ARGV );

# Perform generate
&runGenerateCommand( "-t main -v exclude12.3.v" );

# Simulate and get coverage information
if( $SIMULATOR eq "IV" ) {
  system( "iverilog -DDUMP covered/verilog/exclude12.3.v; ./a.out" ) && die;
} elsif( $SIMULATOR eq "CVER" ) {
  system( "cver -q +define+DUMP covered/verilog/exclude12.3.v" ) && die;
} elsif( $SIMULATOR eq "VCS" ) {
  system( "vcs +define+DUMP covered/verilog/exclude12.3.v; ./simv" ) && die;
} elsif( $SIMULATOR eq "VERIWELL" ) {
  system( "veriwell +define+DUMP covered/verilog/exclude12.3.v" ) && die;
}

# Perform diagnostic running code here
&runScoreCommand( "-vcd exclude12.3.vcd exclude12.3" );

# Create temporary file that will contain an exclusion message
&runCommand( "echo This expression is not needed > exclude12.3.excl" );

# Perform exclusion
&runExcludeCommand( "-m E03 exclude12.3 < exclude12.3.excl" );

# Now print the exclusion
if( $CHECK_MEM_CMD ne "" ) {
  $check = 1;
  $CHECK_MEM_CMD = "";
}
&runExcludeCommand( "-p E03 exclude12.3 > exclude12.3.out" );
if( $check == 1 ) {
  system( "cat exclude12.3.out | ./check_mem > exclude12.3.err" ) && die;
} else {
  system( "mv exclude12.3.out exclude12.3.err" ) && die;
}
system( "cat exclude12.3.err" ) && die;

# Remove temporary exclusion reason file
system( "rm -f exclude12.3.excl" ) && die;

# Perform the file comparison checks
&checkTest( "exclude12.3", 1, 1 );

exit 0;

