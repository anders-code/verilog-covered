#!/usr/bin/env wish

set cdd_name           ""
set uncov_type         1
set cov_type           0
set race_type          0
set mod_inst_type      "module"
set last_mod_inst_type ""

set file_types {
  {{Code Coverage Database Files} {.cdd}}
  {{All Files}                    * }
}

set rpt_file_types {
  {{Covered Report Files} {.rpt}}
  {{All Files}            * }
}

proc do_keybind {.menubar} {

  # Add Mac OS X familiar shortcuts for the menubar
  if {[tk windowingsystem] eq "aqua"} {
    bind all <Command-o> {.menubar.file invoke 0}
    bind all <Command-s> {.menubar.file invoke 3}
    bind all <Command-w> {.menubar.file invoke 4}
  }

  bind all <Control-o> {.menubar.file invoke 0}
  bind all <Control-s> {.menubar.file invoke 3}
  bind all <Control-w> {.menubar.file invoke 4}
  bind all <Control-x> {.menubar.file invoke 8}

  bind all <Control-r> {.menubar.file.gen invoke 0}

  bind all <Control-n> {.menubar.view invoke 0}
  bind all <Control-p> {.menubar.view invoke 1}
  bind all <Control-c> {.menubar.view invoke 2}

}

proc enable_cdd_save {} {

  .menubar.file entryconfigure 3 -state normal

}

proc menu_create {} {

  global start_line end_line
  global cdd_name
  global BROWSER
  global prev_uncov_index next_uncov_index

  # Create the menubar frame
  #frame .menubar -width 710 -relief raised -borderwidth 1
  set mb [menu .menubar -tearoff false]
  . configure -menu $mb

  # Configure the file option
  set tfm [menu $mb.file -tearoff false]
  $mb add cascade -label "File" -menu $tfm

  # FILE - entry 0
  $tfm add command -label "Open/Merge CDDs..." -accelerator "Ctrl-o" -underline 0 -command {
    if {[open_files] != ""} {
      .menubar.file entryconfigure 1 -state normal
      .menubar.file entryconfigure 4 -state normal
      .menubar.file.gen entryconfigure 1 -state normal
    }
  }
  # FILE - entry 1
  $tfm add command -label "View Loaded CDD(s)..." -state disabled -underline 0 -command {
    create_cdd_viewer
  }
  # FILE - entry 2
  $tfm add separator
  # FILE - entry 3
  $tfm add command -label "Save CDD..." -accelerator "Ctrl-s" -state disabled -underline 0 -command {
    set save_name [tk_getSaveFile -filetypes $file_types -initialfile [file tail $cdd_name] -title "Save CDD As"]
    if {$save_name != ""} {
      if {[file extension $save_name] != ".cdd"} {
        set save_name "$save_name.cdd"
      } 
      tcl_func_save_cdd $save_name
      .info configure -text "$save_name successfully saved"
      .menubar.file entryconfigure 3 -state disabled
    }
  }
  # FILE - entry 4
  $tfm add command -label "Close CDD(s)" -accelerator "Ctrl-w" -state disabled -underline 0 -command {
    if {[.menubar.file entrycget 3 -state] == "normal"} {
      set exit_status [tk_messageBox -message "Opened database has changed.  Would you like to save before closing?" \
                       -type yesnocancel -icon warning]
      if {$exit_status == "yes"} {
        .menubar.file invoke 3
      } elseif {$exit_status == "cancel"} {
        return
      }
    }
    tcl_func_close_cdd
    puts "Closed all opened/merged CDD files"
    .info configure -text "$cdd_name closed"
    set cdd_name ""
    clear_cdd_filelist
    populate_listbox
    clear_all_windows
    .menubar.file entryconfigure 1 -state disabled
    .menubar.file entryconfigure 3 -state disabled
    .menubar.file entryconfigure 4 -state disabled
    .menubar.file.gen entryconfigure 1 -state disabled
  }
  # FILE - entry 5
  $tfm add separator
  # FILE - entry 6
  $tfm add cascade -label "Generate" -menu $tfm.gen -underline 0
  menu $tfm.gen -tearoff false
  # FILE-GEN - entry 0
  $tfm.gen add command -label "New CDD..." -accelerator "Ctrl-n" -command {
    create_new_cdd
  }
  $tfm.gen add command -label "ASCII Report..." -accelerator "Ctrl-r" -state disabled -command {
    create_report_generation_window
  }
  $tfm.gen add command -label "CDD Ranking Report..." -accelerator "Ctrl-g" -state disabled -command {
    puts "NEED TO ADD CDD ranking report support"
  }

  # We don't need the exit function if we are running the Aqua version
  if {[tk windowingsystem] ne "aqua"} {
    # FILE - entry 7
    $tfm add separator
    # FILE - entry 8
    $tfm add command -label Exit -accelerator "Ctrl-x" -underline 1 -command {
      if {[.menubar.file entrycget 3 -state] == "normal"} {
        set exit_status [tk_messageBox -message "Opened database has changed.  Would you like to save before exiting?" \
                                       -type yesnocancel -icon warning]
        if {$exit_status == "yes"} {
          .menubar.file invoke 3
        } elseif {$exit_status == "cancel"} {
          return
        }
      }
      exit
    }
  }

  # Configure the report option
  set report [menu $mb.report -tearoff false]
  $mb add cascade -label "Report" -menu $report

  global mod_inst_type cov_uncov_type cov_rb

  $report add radiobutton -label "Module-based"   -variable mod_inst_type -value "module" -underline 0 -command {
    populate_listbox
    .bot.left.tl columnconfigure 1 -hide false
    .menubar.view entryconfigure 4 -label "Hide Summary Module Column" -state disabled
  }
  $report add radiobutton -label "Instance-based" -variable mod_inst_type -value "instance" -underline 1 -state disabled -command {
    populate_listbox
    .menubar.view entryconfigure 4 -state normal
  }
  $report add separator
  $report add checkbutton -label "Show Uncovered" -variable uncov_type -onvalue 1 -offvalue 0 -underline 5 -command {
    set text_x [.bot.right.txt xview]
    set text_y [.bot.right.txt yview]
    if {$cov_rb == "Line"} {
      display_line_cov
    } elseif {$cov_rb == "Toggle"} {
      display_toggle_cov
    } elseif {$cov_rb == "Memory"} {
      display_memory_cov
    } elseif {$cov_rb == "Logic"} {
      display_comb_cov
    } elseif {$cov_rb == "FSM"} {
      display_fsm_cov
    } elseif {$cov_rb == "Assert"} {
      display_assert_cov
    } else {
      # Error
    }
    .bot.right.txt xview moveto [lindex $text_x 0]
    .bot.right.txt yview moveto [lindex $text_y 0]
  }
  $report add checkbutton -label "Show Covered" -variable cov_type -onvalue 1 -offvalue 0 -underline 5 -command {
    set text_x [.bot.right.txt xview]
    set text_y [.bot.right.txt yview]
    if {$cov_rb == "Line"} {
      display_line_cov
    } elseif {$cov_rb == "Toggle"} {
      display_toggle_cov
    } elseif {$cov_rb == "Memory"} {
      display_memory_cov
    } elseif {$cov_rb == "Logic"} {
      display_comb_cov
    } elseif {$cov_rb == "FSM"} {
      display_fsm_cov
    } elseif {$cov_rb == "Assert"} {
      display_assert_cov
    } else {
      # Error
    }
    .bot.right.txt xview moveto [lindex $text_x 0]
    .bot.right.txt yview moveto [lindex $text_y 0]
  }
  $report add checkbutton -label "Show Race Conditions" -variable race_type -onvalue 1 -offvalue 0 -underline 5 -command {
    set text_x [.bot.right.txt xview]
    set text_y [.bot.right.txt yview]
    if {$cov_rb == "Line"} {
      display_line_cov
    } elseif {$cov_rb == "Toggle"} {
      display_toggle_cov
    } elseif {$cov_rb == "Memory"} {
      display_memory_cov
    } elseif {$cov_rb == "Logic"} {
      display_comb_cov
    } elseif {$cov_rb == "FSM"} {
      display_fsm_cov
    } elseif {$cov_rb == "Assert"} {
      display_assert_cov
    } else {
      # Error
    }
    .bot.right.txt xview moveto [lindex $text_x 0]
    .bot.right.txt yview moveto [lindex $text_y 0]
  }
  set mod_inst_type  "module"

  # Configure the color options
  set m [menu $mb.view -tearoff false]
  $mb add cascade -label "View" -menu $m

  $m add command -label "Next Uncovered" -state disabled -accelerator "Ctrl-n" -underline 0 -command {
    goto_uncov $next_uncov_index
  }
  $m add command -label "Previous Uncovered" -state disabled -accelerator "Ctrl-p" -underline 0 -command {
    goto_uncov $prev_uncov_index
  }
  $m add command -label "Show Current Selection" -state disabled -accelerator "Ctrl-c" -underline 5 -command {
    if {$cov_rb == "Toggle"} {
      if {$curr_toggle_ptr != ""} {
        .bot.right.txt see $curr_toggle_ptr.0
      }
    } elseif {$cov_rb == "Logic"} {
      if {$curr_comb_ptr != ""} {
        .bot.right.txt see $curr_comb_ptr.0
      }
    }
  }
  $m add separator
  $m add command -label "Wizard..." -underline 0 -command {
    create_wizard
  }
  # If we are running on Mac OS X, add preferences to applications menu
  if {[tk windowingsystem] eq "aqua"} {
    proc ::tk::mac::ShowPreferences {} { create_preferences -1 }
  } else {
    $m add separator
    $m add command -label "Preferences..." -underline 0 -command {
      create_preferences -1
    }
  }

  # Configure the help option
  set thm [menu $mb.help -tearoff false]
  $mb add cascade -label "Help" -menu $thm

  # Add Manual and About information
  $thm add command -label "Manual" -state disabled -underline 0 -command {
    help_show_manual "contents"
  }
  # Do not display the About information in the help menu if we are running on Mac OS X
  if {[tk windowingsystem] ne "aqua"} {
    $thm add separator
    $thm add command -label "About Covered" -underline 0 -command {
      help_show_about
    }
  }

  # If we are running on Mac OS X, add items to the applications menu
  if {[tk windowingsystem] eq "aqua"} {
    set appl [menu $mb.apple -tearoff false]
    $mb add cascade -menu $appl

    $appl add command -label "About Covered" -command {
      help_show_about
    }
    $appl add separator
  }

  # Enable the manual help entry if we have a browser to use
  if {$BROWSER != ""} {
    $thm entryconfigure 0 -state normal
  }
    
  # Do key bindings for the Top Level Menus
  do_keybind .menubar

}

# Opens/merges a CDD file and handles the GUI cursors and listbox initialization.
proc open_files {} {

  global file_types cdd_name fname global open_type
  global win_cursor txt_cursor e_cursor

  # Get a list of files to open
  if {[catch {tk_getOpenFile -multiple 1 -filetypes $file_types} fnames]} {
    set fnames [tk_getOpenFile -filetypes $file_types]
  }

  # Get all cursor values from various widgets (so we can properly restore them after the open)
  set win_cursor [. cget -cursor]
  set txt_cursor [.bot.right.txt cget -cursor]
  set e_cursor   [.bot.right.h.search.e cget -cursor]

  foreach fname $fnames {

    if {$cdd_name == ""} {
      set open_type "open"
      .info configure -text "Opening $fname..."
      puts "Opening $fname..."
      add_cdd_to_filelist $fname 1
    } else {
      set open_type "merge"
      .info configure -text "Merging $fname..."
      puts "Merging $fname..."
      add_cdd_to_filelist $fname 0
    }

    # Set all widget cursors to the watch
    .                     configure -cursor watch
    .bot.right.txt        configure -cursor watch
    .bot.right.h.search.e configure -cursor watch

    after 100 {

      # Open the CDD file
      if {$open_type == "open"} {
        tcl_func_open_cdd $fname
      } else {
        tcl_func_merge_cdd $fname
        .menubar.file entryconfigure 3 -state normal
      }

      # Populate the listbox
      populate_listbox

    }

    if {$cdd_name == ""} {
      set cdd_name $fname
    }

  }

  # Place new information in the info box
  .info configure -text "Select a module/instance at left for coverage details"

  # Reset the cursors
  .                     configure -cursor $win_cursor
  .bot.right.txt        configure -cursor $txt_cursor
  .bot.right.h.search.e configure -cursor $e_cursor

  return $cdd_name

}

proc create_report_generation_window {} {

  global rptgen_fname rptgen_sel

  if {[winfo exists .rselwin] == 0} {

    toplevel .rselwin
    wm title .rselwin "Create ASCII Report"

    # Create labelframe that will hold the contents
    panedwindow .rselwin.p

    # Initialize global values
    set rptgen_sel   "options"
    set rptgen_fname ""
    
    # Add panes
    .rselwin.p add [create_report_generation_source  .rselwin.p.rs] -width 600 -height 550
    .rselwin.p add [create_report_generation_options .rselwin.p.ro] -width 600 -height 550 -hide true

    # Pack the panedwindow
    pack .rselwin.p -fill both -expand yes
    
  }

  # Finally, raise this window
  raise .rselwin

}

proc read_report_option_file {fname} {
  
  global rsel_wsel rsel_width rsel_sdv rsel_cu rsel_mi rsel_sup
  global rsel_l rsel_t rsel_m rsel_c rsel_f rsel_a rsel_r
  global rsel_fname
    
  if {[catch {set fp [open $rptgen_fname "r"]}]} {
    tk_messageBox -message "File $fname Not Found!" -title "No File" -icon error
  }
 
  set contents [split [read $fp] " \r\b\t\n"]
  puts "contents: $contents"
 
  for {set i 0} {$i<[llength $contents]} {incr i} {
    
    if {[lindex $contents $i] eq "-d"} {
      incr i
      if {[string index [lindex $contents $i] 0] ne "-"} {
        if {[string first [lindex $contents $i] "sdv"] != -1} {
          set rsel_sdv [lindex $contents $i]
        }
      } else {
        set i [expr $i - 1]
      }
      
    } elseif {[lindex $contents $i] eq "-w"} {
      set rsel_wsel 1
      incr i
      if {[string index [lindex $contents $i] 0] ne "-"} {
        if {[string is integer [lindex $contents $i]] == 1} {
          set rsel_width [lindex $contents $i]
        }
      } else {
        set i [expr $i - 1]
      }
      
    } elseif {[lindex $contents $i] eq "-m"} {
      incr i
      if {[string index [lindex $contents $i] 0] ne "-"} {
        foreach $val [split [lindex $contents $i] ""] {
          if {$val eq "l"} {
            set rsel_l "l"
          } elseif {$val eq "t"} {
            set rsel_t "t"
          } elseif {$val eq "m"} {
            set rsel_m "m"
          } elseif {$val eq "c"} {
            set rsel_c "c"
          } elseif {$val eq "f"} {
            set rsel_f "f"
          } elseif {$val eq "a"} {
            set rsel_a "a"
          } elseif {$val eq "r"} {
            set rsel_r "r"
          }
        }
      } else {
        set i [expr $i - 1]
      }
      
    } elseif {[lindex $contents $i] eq "-o"} {
      incr i
      if {[string index [lindex $contents $i] 0] ne "-"} {
        set rsel_fname [lindex $contents $i]
      } else {
        set i [expr $i - 1]
      }
      
    } elseif {[lindex $contents $i] eq "-s"} {
      set rsel_sup "-s"
      
    } elseif {[lindex $contents $i] eq "-i"} {
      set rsel_mi "-i"
      
    } elseif {[lindex $contents $i] eq "-c"} {
      set rsel_cu "-c"
      
    } elseif {[lindex $contents $i] eq "-f"} {
      incr i
      if {[string index [lindex $contents $i] 0] ne "-"} {
        read_report_option_file [lindex $contents $i]
      } else {
        set i [expr $i - 1]
      }
    }
  } 
 
  close $fp
  
}

proc create_report_cmd_options {} {
  
  global rsel_wsel rsel_width rsel_sdv rsel_mi rsel_cu rsel_sup
  global rsel_l rsel_t rsel_m rsel_c rsel_f rsel_a rsel_r
  global rsel_fname cdd_name
  
  # Create command-line to report command of Covered
  if {$rsel_wsel == 0} { set w "" } else { set w "-w $rsel_width" }
  if {$rsel_mi  == "None"} { set mi  "" } else { set mi  " $rsel_mi" }
  if {$rsel_cu  == "None"} { set cu  "" } else { set cu  " $rsel_cu" }
  if {$rsel_sup == "None"} { set sup "" } else { set sup " $rsel_sup" }
  if {$rsel_l   == "None"} { set l   "" } else { set l   " $rsel_l" }
  if {$rsel_t   == "None"} { set t   "" } else { set t   " $rsel_t" }
  if {$rsel_m   == "None"} { set m   "" } else { set m   " $rsel_m" }
  if {$rsel_c   == "None"} { set c   "" } else { set c   " $rsel_c" }
  if {$rsel_f   == "None"} { set f   "" } else { set f   " $rsel_f" }
  if {$rsel_a   == "None"} { set a   "" } else { set a   " $rsel_a" }
  if {$rsel_r   == "None"} { set r   "" } else { set r   " $rsel_r" }
  
  set cmd "-d $rsel_sdv$mi$cu-m $l$t$m$c$f$a$r$w$sup"
  
  return $cmd
  
}

proc create_report {w} {
  
  global rsel_view cdd_name rsel_fname
  
  eval "tcl_func_generate_report [create_report_cmd_options] -o $rsel_fname $cdd_name"
  
  destroy [winfo toplevel $w]
  
  if {$rsel_view == 1} {
    viewer_show rpt {ASCII Report} $rsel_fname
  }
  
}

proc create_report_generation_source {w} {
 
  global rptgen_sel rptgen_fname
  
  # Create the upper widget frame for this pane
  frame $w
  
  # Create upper widgets
  frame $w.f
  radiobutton $w.f.rb_opts -anchor w -text "Create report by interactively selecting options" -variable rptgen_sel -value "options" -command "
    $w.f.l configure -state disabled
    $w.f.e configure -state disabled
    $w.f.b configure -state disabled
  "
  radiobutton $w.f.rb_file -anchor w -text "Create report by using option file" -variable rptgen_sel -value "file" -command "
    $w.f.l configure -state normal
    $w.f.e configure -state normal
    $w.f.b configure -state normal
  "
  label $w.f.l -text "     Option file:" -state disabled
  entry $w.f.e -state disabled -textvariable rptgen_fname
  button $w.f.b -text "Browse..." -state disabled -command {
    set rptgen_fname [tk_getOpenFile -filetypes {{{Command Configuration Files} {.cfg}} {{All Files} *}} -title "Select a Report Option File"]
    puts "B rptgen_fname: $rptgen_fname"
  }
  grid columnconfigure $w.f 1 -weight 1
  grid $w.f.rb_opts -row 0 -column 0 -columnspan 3 -sticky news
  grid $w.f.rb_file -row 1 -column 0 -columnspan 3 -sticky news
  grid $w.f.l       -row 2 -column 0 -sticky news
  grid $w.f.e       -row 2 -column 1 -sticky news
  grid $w.f.b       -row 2 -column 2 -sticky news
  
  # Create button frame
  frame $w.bf
  button $w.bf.help -width 10 -text "Help" -command {
    puts "Help!"
  }
  button $w.bf.cancel -width 10 -text "Cancel" -command "destroy [winfo toplevel $w]"
  button $w.bf.next -width 10 -text "Next" -command "
    puts {A rptgen_fname: $rptgen_fname}
    if {[string length $rptgen_fname] > 0} {
      set rsel_wsel  0
      set rsel_width 105
      set rsel_sdv   s
      set rsel_cu    None
      set rsel_mi    None
      set rsel_sup   None
      set rsel_l     None
      set rsel_t     None
      set rsel_m     None
      set rsel_c     None
      set rsel_f     None
      set rsel_a     None
      set rsel_r     None
      read_report_option_file $rptgen_fname
    } else {
      set rsel_wsel  0
      set rsel_width 105
      set rsel_sdv   s
      set rsel_cu    None
      set rsel_mi    None
      set rsel_sup   None
      set rsel_l     l
      set rsel_t     t
      set rsel_m     m
      set rsel_c     c
      set rsel_f     f
      set rsel_a     None
      set rsel_r     None
    }
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

proc create_report_generation_options {w} {

  global rsel_wsel rsel_width rsel_sup rsel_cu rsel_mi rsel_sdv rsel_view
  global rsel_l rsel_t rsel_m rsel_c rsel_f rsel_a rsel_r
  global rsel_fname cdd_name rsel_sname

  # Create default report filename
  set rsel_fname "[file rootname $cdd_name].rpt"
  set rsel_sname ""

  frame $w

  frame      $w.f
  labelframe $w.f.misc -labelanchor nw -text "Set ASCII Report Generation Options" -padx 4 -pady 6

  # Create width area
  checkbutton $w.f.misc.width_val -text "Limit line width to:" -variable rsel_wsel -anchor w -command "
    if {$rsel_wsel == 0} {
      $w.f.misc.width_w configure -state disabled
    } else {
      $w.f.misc.width_w configure -state normal
    }
  "
  entry $w.f.misc.width_w -textvariable rsel_width -width 3 -validate key -vcmd {string is int %P} -invalidcommand bell -state disabled
  label $w.f.misc.width_lbl -text "characters" -anchor w
  
  # Create empty module/instance suppression area
  checkbutton $w.f.misc.sup_val -text "Suppress modules/instances from output if they contain no coverage information" \
                                        -variable rsel_sup -onvalue "-s" -offvalue "None" -anchor w

  # Allow the user to specify if they would like to view the report after it is generated
  checkbutton $w.f.misc.view -text "View the report in the GUI after it is created" -variable rsel_view -anchor w

  grid $w.f.misc.width_val -row 0 -column 0 -sticky news -pady 4
  grid $w.f.misc.width_w   -row 0 -column 1 -sticky news -pady 4
  grid $w.f.misc.width_lbl -row 0 -column 2 -sticky news -pady 4
  grid $w.f.misc.sup_val   -row 1 -column 0 -columnspan 3 -sticky nw -pady 4
  grid $w.f.misc.view      -row 2 -column 0 -columnspan 3 -sticky nw -pady 4

  # Create and pack detail selection area
  labelframe  $w.f.sdv -text "Level of Detail" -labelanchor nw -padx 4 -pady 6
  radiobutton $w.f.sdv.s -text "Summary"  -variable rsel_sdv -value "s" -anchor w
  radiobutton $w.f.sdv.d -text "Detailed" -variable rsel_sdv -value "d" -anchor w
  radiobutton $w.f.sdv.v -text "Verbose"  -variable rsel_sdv -value "v" -anchor w

  pack $w.f.sdv.s -anchor w
  pack $w.f.sdv.d -anchor w
  pack $w.f.sdv.v -anchor w

  # Create module/instance selection area
  labelframe  $w.f.mi -text "Accumulate By" -labelanchor nw -padx 4 -pady 6
  radiobutton $w.f.mi.m -text "Module"   -variable rsel_mi -value "None" -anchor w
  radiobutton $w.f.mi.i -text "Instance" -variable rsel_mi -value "-i" -anchor w

  pack $w.f.mi.m -anchor w
  pack $w.f.mi.i -anchor w

  # Create metric selection area
  labelframe  $w.f.metric -text "Show Metrics" -labelanchor nw -padx 4 -pady 6
  checkbutton $w.f.metric.l -text "Line"            -variable rsel_l -onvalue "l" -offvalue "None" -anchor w
  checkbutton $w.f.metric.t -text "Toggle"          -variable rsel_t -onvalue "t" -offvalue "None" -anchor w
  checkbutton $w.f.metric.m -text "Memory"          -variable rsel_m -onvalue "m" -offvalue "None" -anchor w
  checkbutton $w.f.metric.c -text "Logic"           -variable rsel_c -onvalue "c" -offvalue "None" -anchor w
  checkbutton $w.f.metric.f -text "FSM"             -variable rsel_f -onvalue "f" -offvalue "None" -anchor w
  checkbutton $w.f.metric.a -text "Assertion"       -variable rsel_a -onvalue "a" -offvalue "None" -anchor w
  checkbutton $w.f.metric.r -text "Race Conditions" -variable rsel_r -onvalue "r" -offvalue "None" -anchor w

  pack $w.f.metric.l -anchor w
  pack $w.f.metric.t -anchor w
  pack $w.f.metric.m -anchor w
  pack $w.f.metric.c -anchor w
  pack $w.f.metric.f -anchor w
  pack $w.f.metric.a -anchor w
  pack $w.f.metric.r -anchor w

  # Create covered/uncovered selection area
  labelframe  $w.f.cu -text "Coverage Type" -labelanchor nw -padx 4 -pady 6
  radiobutton $w.f.cu.u -text "Uncovered" -variable rsel_cu -value "None" -anchor w
  radiobutton $w.f.cu.c -text "Covered"   -variable rsel_cu -value "-c" -anchor w

  pack $w.f.cu.u -anchor w
  pack $w.f.cu.c -anchor w

  # Now pack all of the labelframes
  grid columnconfigure $w.f 2 -weight 1
  grid $w.f.misc   -row 0 -column 0 -columnspan 3 -sticky news -pady 4 -padx 6
  grid $w.f.sdv    -row 1 -column 0               -sticky news -pady 4 -padx 6
  grid $w.f.metric -row 1 -column 2 -rowspan 3    -sticky news -pady 4 -padx 6
  grid $w.f.mi     -row 2 -column 0               -sticky news -pady 4 -padx 6
  grid $w.f.cu     -row 3 -column 0               -sticky news -pady 4 -padx 6
  
  # Create save frame
  frame $w.save
  button $w.save.b -text "Save Options to File" -command {
    set rsel_sname [tk_getSaveFile -title "Save Report Options to File..." -initialfile $rsel_sname]
    if {$rsel_sname ne ""} {
      if {[catch {set fp [open $rsel_sname "w"]}]} {
        tk_messageBox -message "File $sname Not Writable!" -title "No File" -icon error
      }
      puts $fp "[create_report_cmd_options]\n"
      close $fp
    }
  }
  pack $w.save.b -fill x

  # Create filename frame
  frame  $w.fname
  label  $w.fname.lbl -text "Save to file:" -anchor e
  entry  $w.fname.e -textvariable rsel_fname
  button $w.fname.b -text "Browse..." -anchor e -command {
    set rsel_fname [tk_getSaveFile -filetypes $rpt_file_types -initialfile $rsel_fname -title "Save Generated Report As"]
  }
  grid $w.fname.lbl -row 0 -column 0 -sticky news -pady 4
  grid $w.fname.e   -row 0 -column 1 -sticky news -pady 4
  grid $w.fname.b   -row 0 -column 2 -sticky news -pady 4
  grid columnconfigure $w.fname 1 -weight 1

  # Create button frame
  frame  $w.bf
  button $w.bf.back -width 10 -text "Back" -command "goto_prev_pane $w"
  button $w.bf.create -width 10 -text "Create" -command "create_report $w"
  button $w.bf.cancel -width 10 -text "Cancel" -command "destroy [winfo toplevel $w]"
  button $w.bf.help -width 10 -text "Help" -command {
    help_show_manual report_gen
  }
  pack $w.bf.help   -side right -padx 4 -pady 4
  pack $w.bf.cancel -side right -padx 4 -pady 4
  pack $w.bf.create -side right -padx 4 -pady 4
  pack $w.bf.back   -side left  -padx 4 -pady 4

  # Now pack all of the frames
  pack $w.f     -fill both -side top
  pack $w.save  -fill both -expand 1
  pack $w.fname -fill both -expand 1
  pack $w.bf    -fill both -side bottom

  return $w

}

