set dump_filetypes {
  {{VCD Dumpfiles} {.vcd}}
  {{LXT Dumpfiles} {.lxt}}
  {{LXT2 Dumpfiles} {.lxt2}}
  {{All Files}     * }
}

proc create_new_cdd {} {

  global dump_filetypes file_types
  global cdd_filename toplevel_name dumpfile

  # Now create the window and raise it to the front
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
        set_widget_state .newwin.parse    disabled
        set_widget_state .newwin.bot.opts disabled
      } else {
        set_widget_state .newwin.parse    normal
        set_widget_state .newwin.bot.opts normal
      }
      return 1
    }
    button .newwin.general.cdd.b -text "Browse" -command {
      set tmp_cdd [tk_getOpenFile -title "Select CDD Filename" -filetypes $file_types]
      if {$tmp_cdd ne ""} {
        set cdd_filename $tmp_cdd
      }
      raise .newwin
    }
    pack .newwin.general.cdd.l -side left  -fill y
    pack .newwin.general.cdd.e -side left  -fill x -expand 1
    pack .newwin.general.cdd.b -side right -fill y

    # Add dumpfile widgets
    set dumpfile ""
    frame  .newwin.general.dump
    label  .newwin.general.dump.l -text "Dumpfile:"
    entry  .newwin.general.dump.e -textvariable dumpfile
    button .newwin.general.dump.b -text "Browse" -command {
      set dumpfile [tk_getOpenFile -title "Select VCD/LXT Dumpfile" -filetypes $dump_filetypes]
      raise .newwin
    }
    pack .newwin.general.dump.l -side left
    pack .newwin.general.dump.e -side left -fill x -expand 1
    pack .newwin.general.dump.b -side right

    # Pack the general widgets
    pack .newwin.general.cdd  -fill x
    pack .newwin.general.dump -fill x

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

    # Pack the general options frame
    grid columnconfigure .newwin.parse 0 -weight 1
    grid columnconfigure .newwin.parse 1 -weight 1
    grid .newwin.parse.top  -row 0 -column 0 -sticky news
    grid .newwin.parse.inst -row 0 -column 1 -sticky news

    ############################################################
    # Create paned window for verilog paths and console output #
    ############################################################

    panedwindow .newwin.bot -sashrelief raised -sashwidth 4 -orient vertical

    #####################################
    # Add widgets to verilog path frame #
    #####################################

    labelframe .newwin.bot.opts -text "Command-line Options"

    # Create and pack the listbox frame
    frame     .newwin.bot.opts.lbf
    listbox   .newwin.bot.opts.lbf.lb -xscrollcommand {.newwin.bot.opts.lbf.hb set} -yscrollcommand {.newwin.bot.opts.lbf.vb set}
    scrollbar .newwin.bot.opts.lbf.vb -command ".newwin.bot.opts.lbf.lb yview"
    scrollbar .newwin.bot.opts.lbf.hb -orient horizontal -command ".newwin.bot.opts.lbf.lb xview"

    grid rowconfigure    .newwin.bot.opts.lbf 0 -weight 1
    grid columnconfigure .newwin.bot.opts.lbf 0 -weight 1
    grid .newwin.bot.opts.lbf.lb -row 0 -column 0 -sticky news
    grid .newwin.bot.opts.lbf.vb -row 0 -column 1 -sticky ns
    grid .newwin.bot.opts.lbf.hb -row 1 -column 0 -sticky ew

    # Create and pack the button frame
    frame  .newwin.bot.opts.bf
    menubutton .newwin.bot.opts.bf.ins_mb -text "Insert"
    set m [menu .newwin.bot.opts.bf.ins_mb.m -tearoff false]
    .newwin.bot.opts.bf.ins_mb configure -menu $m

    # Add the menu items
    $m add command -label "Source File..." -command {
      set value [tk_getOpenFile -title "Select Verilog Source File"]
      if {$value ne ""} {
        set value "-v $value"
        set index [.newwin.bot.opts.lbf.lb curselection]
        if {$index eq ""} {
          .newwin.bot.opts.lbf.lb insert end $value
        } else {
          .newwin.bot.opts.lbf.lb insert $index $value
        }
      }
    }
    $m add command -label "Library Directory..." -command {
      set value [tk_chooseDirectory -title "Select Verilog Library Directory" -mustexist true]
      if {$value ne ""} {
        set value "-y $value"
        set index [.newwin.bot.opts.lbf.lb curselection]
        if {$index eq ""} {
          .newwin.bot.opts.lbf.lb insert end $value
        } else {
          .newwin.bot.opts.lbf.lb insert $index $value
        }
      }
    }
    $m add command -label "Include Directory..." -command {
      set value [tk_chooseDirectory -title "Select Include Directory" -mustexist true]
      if {$value ne ""} {
        set value "-I $value"
        set index [.newwin.bot.opts.lbf.lb curselection]
        if {$index eq ""} {
          .newwin.bot.opts.lbf.lb insert end $value
        } else {
          .newwin.bot.opts.lbf.lb insert $index $value
        }
      }
    }
    $m add command -label "Define..." -command {
      # TBD
    }
    $m add command -label "Parameter Override..." -command {
      # TBD
    }
    $m add command -label "FSM..." -command {
      # TBD
    }
    $m add separator
    $m add command -label "Module Generation..." -command {
      # TBD
    }
    $m add command -label "Module Exclusion..." -command {
      # TBD
    }
    $m add separator
    $m add command -label "Command File..." -command {
      set value [tk_getOpenFile -title "Select Command File"]
      if {$value ne ""} {
        set value "-f $value"
        set index [.newwin.bot.opts.lbf.lb curselection]
        if {$index eq ""} {
          .newwin.bot.opts.lbf.lb insert end $value
        } else {
          .newwin.bot.opts.lbf.lb insert $index $value
        }
      }
    }

    button .newwin.bot.opts.bf.edit -text "Edit..." -state disabled -command {
      set index [.newwin.bot.opts.lbf.lb curselection]
      set value [split [.newwin.bot.opts.lbf.lb get $index]]
      puts "[lindex $value 1]"
      if {[lindex $value 0] eq "-v"} {
        set value [tk_getOpenFile -title "Select Verilog Source File" -initialfile [lindex $value 1]]
        if {$value ne ""} {
          set value "-v $value"
          .newwin.bot.opts.lbf.lb delete $index
          .newwin.bot.opts.lbf.lb insert $index $value
          .newwin.bot.opts.lbf.lb selection set $index
        }
      } elseif {[lindex $value 0] eq "-y"} {
        set value [tk_chooseDirectory -title "Select Verilog Library Directory" -mustexist true -initialdir [lindex $value 1]]
        if {$value ne ""} {
          set value "-y $value"
          .newwin.bot.opts.lbf.lb delete $index
          .newwin.bot.opts.lbf.lb insert $index $value
          .newwin.bot.opts.lbf.lb selection set $index
        }
      } elseif {[lindex $value 0] eq "-I"} {
        set value [tk_chooseDirectory -title "Select Include Directory" -mustexist true -initialdir [lindex $value 1]]
        if {$value ne ""} {
          set value "-I $value"
          .newwin.bot.opts.lbf.lb delete $index
          .newwin.bot.opts.lbf.lb insert $index $value
          .newwin.bot.opts.lbf.lb selection set $index
        }
      } elseif {[lindex $value 0] eq "-f"} {
        set value [tk_getOpenFile -title "Select Command File" -initialfile [lindex $value 1]]
        if {$value ne ""} {
          set value "-f $value"
          .newwin.bot.opts.lbf.lb delete $index
          .newwin.bot.opts.lbf.lb insert $index $value
          .newwin.bot.opts.lbf.lb selection set $index
        }
      }
    }
    button .newwin.bot.opts.bf.delete -text "Delete" -state disabled -command {
      .newwin.bot.opts.lbf.lb delete [.newwin.bot.opts.lbf.lb curselection]
    }
    bind .newwin.bot.opts.lbf.lb <<ListboxSelect>> {
      if {[.newwin.bot.opts.lbf.lb curselection] ne ""} {
        .newwin.bot.opts.bf.edit   configure -state normal
        .newwin.bot.opts.bf.delete configure -state normal
      }
    }
    pack .newwin.bot.opts.bf.ins_mb -fill x
    pack .newwin.bot.opts.bf.edit   -fill x
    pack .newwin.bot.opts.bf.delete -fill x

    # Now pack the buttons and listbox frames in the labelframe
    pack .newwin.bot.opts.lbf -side left  -fill both -expand 1
    pack .newwin.bot.opts.bf  -side right -fill y

    ################################
    # Add widgets to console frame #
    ################################

    labelframe .newwin.bot.console -text "Console Output"
    text .newwin.bot.console.t -state disabled -xscrollcommand {.newwin.bot.console.hb set} -yscrollcommand {.newwin.bot.console.vb set}
    scrollbar .newwin.bot.console.vb -command {.newwin.bot.console.t yview}
    scrollbar .newwin.bot.console.hb -orient horizontal -command {.newwin.bot.console.t xview}

    grid rowconfigure    .newwin.bot.console 0 -weight 1
    grid columnconfigure .newwin.bot.console 0 -weight 1
    grid .newwin.bot.console.t  -row 0 -column 0 -sticky news
    grid .newwin.bot.console.vb -row 0 -column 1 -sticky ns
    grid .newwin.bot.console.hb -row 1 -column 0 -sticky ew

    # Pack the panedwindow
    .newwin.bot add .newwin.bot.opts
    .newwin.bot add .newwin.bot.console

    #####################################
    # Add widgets to lower button frame #
    #####################################

    frame .newwin.bf
    button .newwin.bf.gen -text "Generate" -state disabled -command {
      puts "Generating CDD file"
    }
    button .newwin.bf.help -text "Help" -command {
      puts "Help"
    }
    button .newwin.bf.cancel -text "Cancel" -command {
      destroy .newwin
    }
    pack .newwin.bf.help   -side right -pady 4 -padx 8
    pack .newwin.bf.gen    -side right -pady 4 -padx 8
    pack .newwin.bf.cancel -side right -pady 4 -padx 8

    # Pack the main frames
    pack .newwin.general -side top    -fill x
    pack .newwin.parse   -side top    -fill x
    pack .newwin.bot     -side top    -fill both -expand 1
    pack .newwin.bf      -side bottom -fill x

  } 

  # Make sure that this window is top-most
  raise .newwin

}

proc set_widget_state {name state} {

  set children [winfo children $name]

  foreach child $children {
    set_widget_state $child $state
  }

  # If this widget has a -state option, set it to the given value
  if {[lsearch -exact [join [$name configure]] {-state}] != -1} {
    $name configure -state $state
  }

}

