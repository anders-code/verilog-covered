# Name:     merge5.pl
# Author:   Trevor Williams  (phase1geo@gmail.com)
# Date:     07/01/2008
# Purpose:  Performs 7 CDD ranking

require "../verilog/regress_subs.pl";

# Initialize the diagnostic environment
&initialize( "merge5", 0, @ARGV );

# Run all diagnostics
if( $USE_VPI == 0 ) {

  &run( "merge5a" );
  &run( "merge5b" );
  &run( "merge5c" );
  &run( "merge5d" );
  &run( "merge5e" );
  &run( "merge5f" );
  &run( "merge5g" );

  # Run the rank command (Note that this is NOT an error)
  &runRankCommand( "-o merge5.err merge5a.cdd merge5b.cdd merge5c.cdd merge5d.cdd merge5e.cdd merge5f.cdd merge5g.cdd" );
  #&runRankCommand( "-o /ptmp/devel/covered/diags/verilog/merge5.err /ptmp/devel/covered/diags/verilog/merge5a.cdd /ptmp/devel/covered/diags/verilog/merge5b.cdd /ptmp/devel/covered/diags/verilog/merge5c.cdd /ptmp/devel/covered/diags/verilog/merge5d.cdd /ptmp/devel/covered/diags/verilog/merge5e.cdd /ptmp/devel/covered/diags/verilog/merge5f.cdd /ptmp/devel/covered/diags/verilog/merge5g.cdd" );

  # Check the difference and remove the CDD files, if necessary
  system( "touch merge5.cdd" ) && die;
  &checkTest( "merge5", 8, 1 );

}

sub run {

  my( $bname ) = $_[0];

  # Simulate the design
  if( $SIMULATOR eq "IV" ) {
    system( "iverilog -DDUMP -y lib ${bname}.v; ./a.out" ) && die; 
  } elsif( $SIMULATOR eq "CVER" ) {
    system( "cver +define+DUMP +libext+.v+ -y lib ${bname}.v" ) && die;
  } elsif( $SIMULATOR eq "VCS" ) {
    system( "vcs +define+DUMP +v2k -sverilog +libext+.v+ -y lib ${bname}.v; ./simv" ) && die; 
  } else {
    die "Illegal SIMULATOR value (${SIMULATOR})\n";
  }

  # Convert configuration file
  if( $DUMPTYPE eq "VCD" ) {
    &convertCfg( "vcd", "${bname}.cfg" );
  } elsif( $DUMPTYPE eq "LXT" ) {
    &convertCfg( "lxt", "${bname}.cfg" );
  } else {
    die "Illegal DUMPTYPE value (${DUMPTYPE})\n";
  }

  # Score CDD file
  &runScoreCommand( "-f ${bname}.cfg" );

  # Check that the CDD file matches
  if( $DUMPTYPE eq "VCD" ) {
    &checkTest( $bname, 0, 0 );
  } else {
    &checkTest( $bname, 0, 5 );
  }

}

exit 0;

