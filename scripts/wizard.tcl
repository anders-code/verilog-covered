proc create_wizard {} {

  # Now create the window and set the grab to this window
  if {[winfo exists .wizwin] == 0} {

    # Create new window
    toplevel .wizwin
    wm title .wizwin "Covered Wizard"
    wm attributes .wizwin -topmost

    frame .wizwin.f -relief raised -borderwidth 1 

    # Create buttons
    button .wizwin.f.new   -text "Generate New CDD File" -command {
    }
    button .wizwin.f.open  -text "Open/Merge CDD File(s)" -command {
    }
    button .wizwin.f.grade -text "Grade CDD Files" -command {
    }
    button .wizwin.f.help  -text "Get Help" -command {
    }

    # Pack the buttons
    grid .wizwin.f.new   -row 0 -column 0 -sticky news -padx 4 -pady 4
    grid .wizwin.f.open  -row 1 -column 0 -sticky news -padx 4 -pady 4
    grid .wizwin.f.grade -row 2 -column 0 -sticky news -padx 4 -pady 4
    grid .wizwin.f.help  -row 3 -column 0 -sticky news -padx 4 -pady 4

    # Pack the frame
    pack .wizwin.f

  }

  grab .wizwin

}
