proc create_new_cdd {} {

  # Now create the window and set the grab to this window
  if {[winfo exists .newwin] == 0} {

    # Create new window
    toplevel .newwin
    wm title .newwin "Create new CDD file"

    # Create top-most frame
    frame .newwin.f -relief raised -borderwidth 1 

  } 

}
