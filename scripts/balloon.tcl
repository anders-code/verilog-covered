################################################################################################
# Copyright (c) 2006-2011 Trevor Williams                                                      #
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

namespace eval balloon {
  variable long_delay 750
  variable short_delay 50
  variable off_delay 5000
  variable delay $long_delay
  variable family {}
  variable after_id {}
}

bind . <Enter> {
  if {$show_tooltips == true} {
    if {$balloon::family != ""} {
      if {[lsearch -exact $balloon::family %W] == -1} {
        set balloon::family {}
        set balloon::delay $balloon::long_delay
      }
    }
  }
}

proc balloon_show {w help} {

  global show_tooltips

  if {$show_tooltips == true} {
    set balloon::after_id [after $balloon::delay [list balloon::show $w $help]]
    set balloon::delay $balloon::short_delay
    set balloon::family [balloon::getwfamily $w]
    set balloon::after_id [after $balloon::off_delay destroy $w.balloon]
  }

}

proc balloon_destroy {w} {

  global show_tooltips

  if {$show_tooltips == true} {
    balloon::hide $w
  }

}

proc set_balloon {w help} {

  bind $w <Enter> "+balloon_show %W [list $help]"
  bind $w <Leave> "+balloon_destroy %W"
  bind $w <Button> "+balloon_destroy %W"

}

# Add these to the namespace
proc balloon::getwfamily {w} {
  return [winfo children [winfo parent $w]]
}

proc balloon::show {w arg {bgcolor lightyellow} {fgcolor black}} {

  if {[string compare -length [string length $w] [eval winfo containing  [winfo pointerxy .]] $w] != 0} {return}

  after cancel $balloon::after_id
  set top $w.balloon
  catch {destroy $top}
  toplevel $top -bd 1 -bg black
  wm overrideredirect $top 1
  if {[string equal [tk windowingsystem] aqua]}  {
    ::tk::unsupported::MacWindowStyle style $top help none
  }
  pack [message $top.txt -aspect 1500 -bg $bgcolor -fg $fgcolor -padx 1 -pady 0 -text $arg]
  set wmx [winfo pointerx $w]
  set wmy [expr [winfo pointery $w]+20]
  wm geometry $top [winfo reqwidth $top.txt]x[winfo reqheight $top.txt]+$wmx+$wmy
  raise $top

}

proc balloon::hide {w} {
  destroy $w.balloon
}
