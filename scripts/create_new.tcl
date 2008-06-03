set dump_filetypes {
  {{VCD Dumpfiles} {.vcd}}
  {{LXT Dumpfiles} {.lxt}}
  {{LXT2 Dumpfiles} {.lxt2}}
  {{All Files}     * }
}

proc create_new_cdd {} {

  global dump_filetypes file_types
  global cdd_filename toplevel_name dumpfile

  # Now create the window and set the grab to this window
  if {[winfo exists .newwin] == 0} {

    # Create new window
    toplevel .newwin
    wm title .newwin "Create new CDD file"

    ############################################
    # Add widgets to general option item frame #
    ############################################

    labelframe .newwin.general -text "General CDD Options"

    # Add the CDD filename widgets
    set cdd_filename ""
    frame  .newwin.general.cdd
    label  .newwin.general.cdd.l -text "CDD name:"
    entry  .newwin.general.cdd.e -textvariable cdd_filename -validate all -vcmd {
      if {[file isfile $cdd_filename]} {
        set_widget_state .newwin.parse disabled
      } else {
        set_widget_state .newwin.parse normal
      }
    }
    button .newwin.general.cdd.b -text "Browse" -command {
      set cdd_filename [tk_getOpenFile -title "Select CDD Filename" -filetypes $file_types]
      if {$cdd_filename ne ""} {
        # Disable all of the items in the design parser labelframe
        puts [winfo children .newwin.parse]
      }
      raise .newwin
    }
    pack .newwin.general.cdd.l -side left  -fill y
    pack .newwin.general.cdd.e -side left  -fill x -expand 1
    pack .newwin.general.cdd.b -side right -fill y

    # Pack the general widgets
    pack .newwin.general.cdd -fill both

    ############################################
    # Add widgets to design parsing item frame #
    ############################################

    labelframe .newwin.parse -text "Design parsing fields"

    # Add toplevel design name widgets
    set toplevel_name ""
    frame .newwin.parse.top
    label .newwin.parse.top.l -text "Toplevel module name:"
    entry .newwin.parse.top.e -textvariable toplevel_name
    pack  .newwin.parse.top.l -side left
    pack  .newwin.parse.top.e -side left -fill x -expand 1

    # Add root pathname widgets
    set inst_name ""
    frame .newwin.parse.inst
    label .newwin.parse.inst.l -text "Root pathname:"
    entry .newwin.parse.inst.e -textvariable inst_name
    pack  .newwin.parse.inst.l -side left
    pack  .newwin.parse.inst.e -side left -fill x -expand 1

    # Add dumpfile widgets
    set dumpfile ""
    frame  .newwin.parse.dump
    label  .newwin.parse.dump.l -text "Dumpfile:"
    entry  .newwin.parse.dump.e -textvariable dumpfile
    button .newwin.parse.dump.b -text "Browse" -command {
      set dumpfile [tk_getOpenFile -title "Select VCD/LXT Dumpfile" -filetypes $dump_filetypes]
      raise .newwin
    }
    pack .newwin.parse.dump.l -side left
    pack .newwin.parse.dump.e -side left -fill x -expand 1
    pack .newwin.parse.dump.b -side right

    # Pack the general options frame
    grid .newwin.parse.top  -row 0 -column 0 -sticky news
    grid .newwin.parse.inst -row 0 -column 1 -sticky news
    grid .newwin.parse.dump -row 1 -column 0 -sticky news -columnspan 2

    ############################################################
    # Create paned window for verilog paths and console output #
    ############################################################
    panedwindow .newwin.bot -orient horizontal

    #####################################
    # Add widgets to verilog path frame #
    #####################################

    labelframe .newwin.bot.vpaths -text "Verilog Libraries/Files"

    ################################
    # Add widgets to console frame #
    ################################

    labelframe .newwin.bot.console -text "Console Output"
    text .newwin.bot.console.t -state disabled
    pack .newwin.bot.console.t -fill both

    # Pack the panedwindow
    .newwin.bot add .newwin.bot.vpaths .newwin.bot.console

    # Pack the main frames
    pack .newwin.general -fill x
    pack .newwin.parse   -fill x
    pack .newwin.bot     -fill both

  } 

  # Make sure that this window is top-most
  raise .newwin
  grab .newwin

}

proc set_widget_state {name state} {

  set children [winfo children $name]

  foreach child $children {
    set_widget_state $child $state
  }

  puts "$name:  [$name configure]"

  # If this widget has a -state option, set it to the given value
  if {[lsearch [$name configure] {-state}] ne ""} {
    $name configure -state $state
  }

}

