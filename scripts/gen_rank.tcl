proc create_rank_cdds {} {

  global rankgen_sel rankgen_fname

  if {[winfo exists .rankwin] == 0} {

    toplevel .rankwin
    wm title .rankwin "Create CDD Coverage Ranking"
    wm transient .rankwin .

    # Create labelframe that will hold the contents
    panedwindow .rankwin.p

    # Add panes
    .rankwin.p add [create_rank_cdds_source  .rankwin.p.source] -width 750 -height 450
    .rankwin.p add [create_rank_cdds_options .rankwin.p.output] -width 750 -height 450 -hide true

    # Initialize global variables
    set rankgen_sel   "options"
    set rankgen_fname ""

    # Pack the panedwindow
    pack .rankwin.p -fill both -expand yes

  }

  # Finally, raise this window
  raise .rankwin

}

proc read_rank_option_file {w fname} {

  if {[catch {set fp [open $fname "r"]}]} {
    tk_messageBox -message "File $fname Not Found!" -title "No File" -icon error
  }

  set contents [join [list [read $fp]]]

  for {set i 0} {$i<[llength $contents]} {incr i} { 

    if {[lindex $contents $i] eq "-o"} {
      incr i
      if {[string index [lindex $contents $i] 0] ne "-"} {
        set rank_filename [lindex $contents $i]
      } else {
        set i [expr $i - 1]
      }

    }

  }

  close $fp

}

proc setup_cdd_rank_options {w} {

  global rankgen_fname

  if {$rankgen_fname ne ""} {

    # Perform global variable initialization here

    read_rank_option_file $w $rankgen_fname

  } else {

    # Otherwise, set global variables to desired default values

  }

}

proc create_rank_cdds_source {w} {

  global rankgen_sel rankgen_fname

  # Create the upper widget frame for this pane
  frame $w

  # Create upper widgets
  frame $w.f
  frame $w.f.fu
  frame $w.f.fc
  frame $w.f.fl
  radiobutton $w.f.fc.rb_opts -anchor w -text "Create CDD ranking by interactively selecting options" -variable rankgen_sel -value "options" -command "
    $w.f.fc.e configure -state disabled
    $w.f.fc.b configure -state disabled
  "
  radiobutton $w.f.fc.rb_file -anchor w -text "Create CDD ranking by using option file" -variable rankgen_sel -value "file" -command "
    $w.f.fc.e configure -state normal
    $w.f.fc.b configure -state normal
  "
  entry  $w.f.fc.e -state disabled -textvariable rankgen_fname
  button $w.f.fc.b -text "Browse..." -state disabled -command {
    set fname [tk_getOpenFile -title "Select a Rank Command Option File" -parent .rankwin]
    if {$fname ne ""} {
      set rankgen_fname $fname
    }
  }
  grid columnconfigure $w.f.fc 1 -weight 1
  grid $w.f.fc.rb_opts -row 0 -column 0 -columnspan 3 -sticky news -pady 10
  grid $w.f.fc.rb_file -row 1 -column 0 -sticky news -pady 10
  grid $w.f.fc.e       -row 1 -column 1 -sticky news -pady 10
  grid $w.f.fc.b       -row 1 -column 2 -sticky news -pady 10

  pack $w.f.fu -fill both -expand 1
  pack $w.f.fc -fill x
  pack $w.f.fl -fill both -expand 1

  # Create button frame
  frame $w.bf
  button $w.bf.help   -width 10 -text "Help" -command {
    puts "Help!"
  }
  button $w.bf.cancel -width 10 -text "Cancel" -command "destroy [winfo toplevel $w]"
  button $w.bf.next   -width 10 -text "Next" -command "
    setup_cdd_rank_options $w
    goto_next_pane $w
  "
  pack $w.bf.help   -side right -padx 4 -pady 4
  pack $w.bf.cancel -side right -padx 4 -pady 4
  pack $w.bf.next   -side right -padx 4 -pady 4

  # Pack top-level frames
  pack $w.f  -fill both -expand yes
  pack $w.bf -fill x

  return $w

}

proc create_rank_cdds_options {w} {

  # Create top-most frame
  frame $w

  return $w

}
