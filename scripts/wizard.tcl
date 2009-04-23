################################################################################################
# Copyright (c) 2006-2009 Trevor Williams                                                      #
#                                                                                              #
# This program is free software; you can redistribute it and/or modify                         #
# it under the terms of the GNU General Public License as published by the Free Software       #
# Foundation; either version 2 of the License, or (at your option) any later version.          #
#                                                                                              #
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;    #
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.    #
# See the GNU General Public License for more details.                                         #
#                                                                                              #
# You should have received a copy of the GNU General Public License along with this program;   #
# if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. #
################################################################################################

proc create_wizard {} {

  global show_wizard HOME

  # Now create the window and set the grab to this window
  if {[winfo exists .wizwin] == 0} {

    # Create new window
    toplevel .wizwin
    wm title .wizwin "Covered Wizard"

    frame .wizwin.f -relief raised -borderwidth 1 

    # Add banner
    label .wizwin.f.banner -image [image create photo -file [file join $HOME scripts banner.gif]]

    # Create buttons
    button .wizwin.f.new   -text "Generate New CDD File" -relief flat -command {
      create_new_cdd
      destroy .wizwin
    }
    button .wizwin.f.open  -text "Open/Merge CDD File(s)" -relief flat -command {
      .menubar.file invoke 0
      destroy .wizwin
    }
    button .wizwin.f.grade -text "Rank CDD Coverage" -relief flat -command {
      create_rank_cdds
      destroy .wizwin
    }
    button .wizwin.f.help  -text "View Help Manual" -relief flat -command {
      help_show_manual part.gui 
    }

    # Checkbutton to disable this window
    checkbutton .wizwin.f.show -text "Display this window at startup of Covered" -variable show_wizard -onvalue true -offvalue false -command {
      write_coveredrc 0
    }

    help_button .wizwin.f.hb chapter.gui.wizard 

    # Pack the buttons
    grid .wizwin.f.banner -row 0 -rowspan 4 -column 0 -sticky news -padx 4 -pady 4
    grid .wizwin.f.new    -row 0 -column 1 -sticky news -padx 4 -pady 4
    grid .wizwin.f.open   -row 1 -column 1 -sticky news -padx 4 -pady 4
    grid .wizwin.f.grade  -row 2 -column 1 -sticky news -padx 4 -pady 4
    grid .wizwin.f.help   -row 3 -column 1 -sticky news -padx 4 -pady 4
    grid .wizwin.f.show   -row 4 -column 0 -sticky news -padx 4 -pady 4
    grid .wizwin.f.hb     -row 4 -column 1 -sticky e -pady 4

    # Pack the frame
    pack .wizwin.f -fill both -expand yes

  }

  # Make sure that the focus is on this window only
  wm transient .wizwin .

}
