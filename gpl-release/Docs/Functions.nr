.CH "The List of Commands"
.CS "The Functionality of Conquer"
.PP
Almost all of the commands within Conquer have been given function
names.  These names may be used to reassign keybindings, adjusting the
feel of Conquer to fit other preferences.  In general, Conquer
bindings are based on emacs and/or vi keys, so most users will be able
to adjust quickly to the default keybindings.
.PP
Conquer consists of a number of different keybinding maps, for a total
of seven different modes.  The "global" commands cover all of the top
level Conquer functions available while looking at the main map
window.  The "move" keymap handles all of the bindings during unit or
civilian relocation.  The "magic" commands deal with those functions
used while perusing or purchasing magical powers.  The "xfer" keymap
is for the screen where goods, units, and people are transfered
between two storage locations.  The "reader" keymap is for use in the
mail reader.  The "mail" keymap is for when a new mail message is
being written.  Finally, the "info" keymap is used for keybindings in
the nation information screen.
.PP
Again, if any of the default key bindings are unsuitable, they may be
adjusted using the customization system built into Conquer.
.RF "Customize" "Conquer Customization Settings" 
.PP
At any time, help may be obtained using `?' (`ESC-?' if in the "mail"
editor).  This help listing will provide access to a listing of all of
the commands and keybindings in the current keymap.
.CE
.CS "The Global Function Keys"
.PP
The "global" keymap provides the overall control for Conquer, allowing
players to access all of the other keymaps and functions.  The other
purpose of the "global" keymap is to select military units within the
current sector, or to move between the various sectors.
.PP
The movement commands are available with both the top-level keymap and
the "move" keymap.  But, within the "move" keymap the scrolling
commands are not available.
.PP
Selection of units is very important, since a unit will need to be
selected before orders may be given to it.  There are a number of
commands which handle this task.
.PP
Finally, with the display and digestion of information being very
important in game play, there are numerous functions available to
select what sort of information is displayed on the screen.  Most of
this information is directly available at the "global" level, with
precise details being available in some of the other keymaps.
.CE
.CS "The Movement Commands"
.PP
Of all of the commands available to the players of Conquer, those
which control the selection of the current sector will get the most
use, and as such, should be memorized first.
.PP
By default, the VI keys (hjklyubn) are set to all of the motion
commands.  The upper case equivalents provide functions which scroll
the screen in the given direction.  Also, the numeric keypad is
available for those that wish to use that alternative.  For the
numeric keypad, you may use a period followed by the number to scroll
in the appropriate direction.
.PP
The directions provided by the standard VI keys and numeric keypad is
shown using the following diagrams:
.SB
               `y' `k' `u'                     `7' `8' `9'
                 \e  |  /                         \e  |  /   
                  \e | /                           \e | /    
              `h'--` '--`l'                   `4'--` '--`6'
                  / | \e                           / | \e 
                 /  |  \e                         /  |  \e
               `b' `j' `n'                     `1' `2' `3'
.sp
          Upper Case to Scroll             Use ".#" to Scroll
.SE
.PP
The full names of the movement functions are of the form
"go-direction", such as "go-north", "go-southwest".  The scrolling
functions are likewise, "scroll-direction".
.PP
Note that with a hexagonal map, the directions west and east are not
available during the movement mode, since there are only six compass
directions.
.CE
.CS "Display Commands"
.PP
During the normal interface mode of Conquer, the map of the world, or
a portion of that map, is displayed to the player.  For each sector,
there is a good deal of information which may be obtained.  In order
to quickly digest the large amount of data within the world, the
global view of the map can be changed to show different information to
the user.  The "display" commands provide the method for making such
changes.  The commands, with their default bindings within
parenthesis, are as follows:
.IT
adjust-display (`d') -- Select which display method is used to show
the information on the screen.
.IT
customize-display (`D') -- Create new display modes or modify the
existing display modes to your taste.
.IT
adjust-view (`^D') -- Change the character set for either the
vegetation, the major designations, or the contour of the land.  This
command is useful to allow a clearer distinction between different
vegetations, designations, or elevations.
.IT
shift-focus (`)',`+') -- Rotate the focus position clockwise.
.IT
shift-focus-back (`(') -- Rotate the focus position counter-clockwise.
.IT
highlight-all (`=') -- Change all of the highlighting characters.
.IT
highlight-current (`,') -- Change the highlight at the focus character.
.IT
highlight-horizontal (`-') -- Change the highlight of the focus and
neighboring character.
.IT
highlight-upleft-lowright (`\e') -- Change the highlight of the upper
left and lower right characters.
.IT
highlight-upright-lowleft (`/') -- Change the highlight of the upper
right and lower left characters.
.IT
highlight-vertical (`|') -- Change the highlight of the current character
and the character above or below.
.IT
troop-listing (TAB) -- Display a list of all troops under the control
of other nations which are visible in the current sector.
.IT
toggle-infomode (`@') -- Toggle between the standard Conquer map
display and a mode which provides more information about the current
sector.
.IT
zoom-in (`>') -- If possible, give a more detailed look at the sectors
on the map.
.IT
zoom-out (`<') -- Enlarge the scale of the map, if possible, and
display more sectors than are currently shown.
.UT
.PP
It should be noted that the `^D' construct indicates a Control-D
character, which is obtained by holding down the "Control" key and
then pressing the `D' key.
.PP
Details concerning the display commands can be found elsewhere in the
documentation.
.RF "Display" "Conquer Data Display Routines"
.CE
.CS "Selection Commands"
.PP
Selecting a specific military unit or sector is necessary before that
unit or sector may receive commands.  The current sector is indicated
on the map portion of the screen by the placement of the cursor, and
information about the sector will be displayed in the non-map portions
of the screen.  If any units are in the current sector they will be
listed on the right side of the screen.  The unit which is highlighted
and has a character other than the normal "greater than" sign next to
it is considered the "current" unit.  [The character used will be
either a plus sign or an asterisk.]
.PP
The commands available for changing selections are:
.IT
army-choose (`^A',`^G') -- Set the current unit to be a given army
unit.  This will adjust the current sector to the location of the
army, if it is not already within the current sector.
.IT
army-next (`A',`G') -- Set the current unit to be the next army unit
(in numerical order).  If needed, the current sector will also be
adjusted.
.IT
caravan-choose (`^V') -- Set the current unit to be a given caravan.
Again, the sector of focus is adjusted if the unit is not within the
current sector.
.IT
caravan-next (`V') -- Set the current unit to be the numerically next
caravan unit.  The current sector will be adjusted if the unit is not
already within the current sector.
.IT
city-choose (`^E') -- Relocate the current sector to the city of
choice within the nation.
.IT
jump-to-capital (`X') -- Set the current sector to be capital of the
nation.
.IT
jump-to-mark (`^X') -- Adjust the current sector to a previously
stored location (see the "mark-sector" command) or to the national
capital if one has not been stored.
.IT
jump-to-pick (`x') -- Set the current sector to a specified sector,
which is entered by coordinates.
.IT
mark-sector (NULL-key) -- This command will mark the current sector
so that it may later be returned to using the "jump-to-mark" command.
.IT
navy-choose (`^F') -- Set the current unit to be a given naval fleet.
Again, the sector of focus is adjusted if the unit is not within the
current sector.
.IT
navy-next (`F') -- Set the current unit to be the next, in ascending
numerical order, naval fleet.  The current sector will be adjusted if
need be.
.IT
pick-next (`p') -- Select the next unit by displayed order within the
current sector, regardless of unit type.  This command will never
change the current sector.
.IT
pick-previous (`o') -- Select the preceding unit within the current
sector, as indicated by the display.  This command will also never
cause the current sector to be changed.
.UT
.PP
Note that the NULL-key is usually defined to be `^@' or `^ '
(Control-Space).
.CE
.CS "Unit Manipulation Commands"
.PP
Within Conquer, there are three types of military units: army, navy,
and caravan units.  Since the actions of a nation are greatly
dependent on the power, location, and grouping of its military, there
are a number of commands available to allow the manipulation of such
units:
.IT
army-report (`a') -- Display a listing of all of the armies within the
nation, and allow the manipulation of army units while examining the
summary.
.IT
auto-army-numbering (`#') -- The choice of how to number army units
can make selection and identification of army units easier.
.IT
caravan-report (`v') -- Summarize the data concerning all of the
caravans within the nation, and allow the adjustment of caravans while
examining such data.
.IT
command-unit (ESC-key) -- Perform a quick manipulation of the
currently selected unit (army, navy, or caravan).  Some of the options
include:  adjusting the speed, renumbering the unit, changing the unit
status, setting the supply level, etc.
.IT
enlist (`E') -- Create a new army, navy or caravan from the people and
resources within the sector.  The current sector must be a stockade,
town, city or capital for it to enlist troops.  Drafted troops are
taken from the population of the sector.  Upgrading of units is also
possible using this command.
.IT
group-report (`g') -- This command provides a summary of the armies
within the nation, just like the "army-report" command, but it limits
the data to only those armies within the current sector.
.IT
move-unit (`m') -- The is one of the most useful commands within the
program, as it allows the player to relocate a unit from one sector to
another.  The cost and method of unit movement will vary with the type
of unit, terrain and nation abilities.
.RF "Movement" "Unit Relocation"
.IT
navy-report (`f') -- This command will provide a summary of all of the
naval fleets within the nation, and allow the manipulation of the
units while looking at the data.
.IT
transfer (`t') -- Move items between a caravan or navy and a city or
another caravan or navy.  The currently selected unit will be used as
the target of the transfer.  Goods may also be transfered directly
between cities using the "region-command".
.RF "Economics" "Economic Management"
.UT
.PP
Normally the ESC-key is a single button, or `^[' (Control-Left
Bracket).
.CE
.CS "Informational Commands"
.PP
In addition to having a need to manipulate the different military
units of your nation, players must be provided with an organized
method for retrieving all of the information concerning their nation
and the world around them.  The army, navy, and caravan reports are
examples of just such commands.  Some more commands for obtaining
information are:
.IT
campaign-info (`I') -- This command will provide a summary of the
settings for this particular campaign.  Most importantly this screen
will indicate many of the settings which were used when the world was
created, giving the player an idea of the difficulty of the
environment around the nation.
.IT
conquer-options (`0') -- Through the conquer-options command, the
player is able to adjust keybindings, and choose a new relative center
for the nation.
.RF "Customize" "Conquer Customization Settings"
.IT
diplomacy-report (`S') -- This command is used to review what nations
you have met and what your diplomatic status is towards these nations.
Here is where you may adjust the status of your nation towards other
nations, declaring war or offering a treaty.
.RF "Nations" "Racial Traits and Nation Roles"
.IT
economy-report (`e') -- Careful monitoring of the economic prosperity
of your nation is necessary for your survival, and this command is the
quickest means for obtaining a report concerning your nation on a
global or regional basis.
.RF "Economics" "Economic Management"
.IT
help (`?') -- List all of the functions and associated keybindings.
.IT
magic-info (`W') -- Display the list of your national powers, as well
as provide a method for gaining more.
.RF "Powers" "Magical Powers"
.IT
nation-info (`i') -- There are a number of national parameters which
can give a good indication as to the strength and well being of your
nation.  This command provides access to all such parameters governing
your nation, and allows you to adjust those which within your
immediate control.
.RF "Nations" "Racial Traits and Nation Roles"
.IT
toggle-infomode (`@') -- Mentioned under the display commands, this
information providing function was worth mentioning again.
.RF "Display" "Conquer Data Display Routines"
.IT
troop-listing (TAB) -- Also mentioned under the display commands, this
command really isn't worth mentioning a second time, but I thought I
would anyway.
.IT
world-scores (`s') -- It is always a good idea to keep an eye on what
other people are up to.  This command gives a short summary of all of
the nations in the world and, if your campaign allows it, a
description of their strengths.
.UT
.CE
.CS "Communication Commands"
.PP
Conquer has been designed as a multi-player fantasy wargame, with much
of the emphasis being on the multi-player portion.  As such,
communication with other nations is very important, and there are a
few commands available to facilitate that:
.IT
mail-nation (`M') -- Send mail to another nation.
.IT
read-mail (`R') -- Read, reply to, and forward mail send to your
nation.
.IT
read-paper (`P') -- Check what is happening in the world around you.
.UT
.PP
A more detailed description of the mail facility can be found
elsewhere in the documentation.
.RF "Xchanges" "Communication Between Players"
.CE
.CS "Nation Control Commands"
.PP
Being able to manipulate the world around you can give such a nice
feeling.  So, in order to allow the player to get that feeling of
power that we all need in our lives, there are a few commands
available to the ruler of a nation which allow the manipulation of the
population and the land:
.IT
change-designation (`c') -- Adjust the designation for the current
sector.
.RF "Sectors" "Sector Information"
.IT
construct (`C') -- Add to the value of a sector by building things
such as fortifications, churches, granaries, or any other minor
designations.
.RF "Sectors" "Sector Information"
.IT
move-people (`Z') -- Relocate civilians at a cost of 50 talons per
person to within a two sector radius.
.RF "Movement" "Unit Relocation"
.IT
region-command (`r') -- Rename a supply center, change weighting,
distribute supplies, or transfer raw materials.
.RF "Economics" "Economic Management"
.UT
.CE
.CS "Miscellaneous Commands"
.PP
Some functions just can't be put into any category:
.IT
ignore-key (` ') -- Well, this command does just what it says: the key
will be ignored if it is hit.  This is useful to avoid having a bell
ring everytime you hit a key which you already know is useless.
.IT
recenter-screen (`^R') -- This command will cause the current sector
to be centered within the map portion of the Conquer display.
.IT
redraw-screen (`^L') -- This command will cause the screen to be
redrawn.
.UT
.PP
Both the "ignore-key" and "redraw-screen" functions should be
available through all of the Conquer key-maps.
.CE
.CS "Is this the end?"
.PP
Well, there is still the big question: How do I get out of Conquer?
For some people, once you are hooked, forget it.  But, for those few
who can actually stop playing, there are the following commands:
.IT
login-nation (`z') -- Switch control to another nation, continuing the
addiction.
.IT
quit (`q') -- Okay, so you do want out.  This is the command to do it.
.IT
quit-and-save (`Q') -- This command not only lets you out, but informs
Conquer that you are ready for the game to be updated.  When all
players have indicated such, early updates might be able to take
place.  [For now this does nothing more than the "quit" function]
.CE
