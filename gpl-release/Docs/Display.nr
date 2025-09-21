.CH "Conquer Data Display Routines"
.CS "Cartography in Conquer"
.PP
With Conquer version 5.0, hexagonal sectors and relative coordinate
systems were introduced.  Combined, these changes can be more than a
bit disorienting for players of the older versions of Conquer.  But,
both of these changes greatly enhance game play by introducing more
realism.
.PP
The shape of the sectors may be selected during world creation, with
the hexagonal map being the recommended choice.  With this setup,
motion in any of the six directions results in a relocation of equal
distance.  With rectangular sectors, distances become non-uniform, as
can be demonstrated by having motion one sector east followed by one
sector north being equivalent to motion one sector to the north-east.
.PP
The numbering of coordinates is along both the north-south axis and
the east-west axis.  This can be somewhat confusing for the hexagonal
coordinate system, with there then being zig-zagging rows, but this
provides a one-to-one relationship between the displayed map
coordinates and the internal data representation used.  Thus, some
complex computations are avoided.
.CE
.CS "The Displaying of Data"
.PP
With information a key element to game play, being able to quickly
view data is a must.  The display system in Conquer is designed to
allow concise viewing of data through character representations and
display highlighting.
.PP
There are three levels of detail available for the map display.  The
farthest out displays the most sectors, but the closest in has a
display of up to four characters for each sector.  This allows for a
greater amount of data for each sector to be shown.  When on the
detailed zoom setting, one of the four characters is designated as the
focus character and is indicated by the positioning of the cursor
within the sector.  There are a number of different types of data
which may be shown in each character slot, and there are also many
methods of highlighting which may be employed upon each character
slot.
.PP
It is possible to build different combinations of character displays
and highlights to provide informative methods of gathering data.  The
grouping of such display and highlight settings is called a "display
mode".  There are a number of pre-built "display modes", or others may
be constructed using the display editing commands.
.CE
.CS "Display Commands"
.PP
There are a number of commands available to allow for the adjustment
of the display settings.  These commands range from zooming controls
and information mode toggling to outright editing modes for display
settings.
.PP
The zooming commands are "zoom-in" and "zoom-out" which slide between
the various zooming levels.  To shift the focus between the four
character slots, there are the "shift-focus" and "shift-focus-back"
commands which rotate the focus clockwise and counter-clockwise
respectively.
.PP
For those times when you want to get more information about the
current sector (such as production, consumption, and minor
designations) there is the "info-mode".  When running under the
info-mode, the main conquer display devotes a larger portion of the
screen to show information concerning the current sector.  The right
side of the screen is totally devoted to listing the military units in
the sector, and the map portion is relegated to a smaller section of
the screen, in the upper left.  The "toggle-infomode" command is used
to switch between this mode and the normal conquer map display.
.PP
One quick command which can sometimes come in handy is
"troop-listing".  This command clears the entire right column of the
screen and then uses this space to list all troops belonging to other
nations within that space.  This is useful whenever there are so many
troops within the sector that they do not fit within what little space
is leftover after the listing of your own troops.
.PP
There are two commands which provide customizing of the display
routines.  The simplest is the "adjust-view" command which allows the
user to change the symbols used to represent the designation,
vegetation or contour values.  Simply enter the selection to be
changed and then give a printable character to use for the display.
The much more complex command, "customize-display" allows interactive
creation or editing of new display modes.  Once the display mode has
been created, it may be selected using the "adjust-display" command.
.PP
Finally, because the highlighting of certain sectors can be a very
quick method for interpreting data, there are a number of commands
that allow the quick adjustment of the highlighting patterns.  Those
commands are: "highlight-all", "highlight-current",
"highlight-horizontal", "highlight-upleft-lowright",
"highlight-upright-lowleft", and "highlight-vertical".
.PP
The two pages which follow give the complete list of all of the
display and highlighting styles which conquer supports.  These lists
are used as the basis for the display modes themselves.
.CE
.CS "List of Display Styles"
.IT
(c)ontour -- Show the elevation for all of the sectors on the screen.
.IT
(d)esignation -- Display the major designation value for any sectors
whose designation is known.
.IT
(f)ood -- Display the food production ability for all habitable
sectors.
.IT
(j)ewel -- Show the approximate jewel production ability of all
habitable sectors.
.IT
(m)etal -- Show the approximate metal production ability of all
habitable sectors.
.IT
(n)ation mark -- Display the nation mark of the owner for all owned
sectors.
.IT
(p)eople -- Display the approximate population level for owned
sectors.  Arabic numerals (0 to 9), indicate hundreds of people, and
Roman numerals (I, V, X, L) indicate thousands of people.
.IT
(r)ace -- Show the first letter of the race of the sector owner.  The
races are: Human, Dwarf, Elf, Orc, Nomad, Pirate, Savage and Lizard.
.IT
(v)egetation -- Show the type of vegetation contained within a sector.
.IT
(w)ood -- Show the wood production ability for those sectors able to
produce wood.
.IT
(y)our desg -- Display designation for sectors owned by the current
nation, and the nation marks for others.
.IT
(A)rmy mcost -- Display the relative cost for a land based unit to
enter each sector.  This cost is used for caravan units as well as
army units.
.IT
(D)efense -- Show defensive value for a sector based on the vegetation
and elevation of the sector.
.IT
(F)light mcost -- Display the cost, in movement points, for a flying
unit to enter each sector.
.IT
(M)agic value -- Display the magical potential of the sectors.
.IT
(N)aval mcost -- Show the cost, in movement points, for a ship to
enter a sector.  A `+' indicates that the sector may only be entered
by landing the ship.
.IT
(T)good desgs -- Display designations necessary to take advantage of
the special items in sectors.
.IT
(V)alues -- Show tradegood values for each sector.  With jewel,
magical, and metal tradegoods, the "mineral" value is shown, while
other tradegood types will have the potency of the tradegood shown.
.IT
(W)eights -- Display the relative weighting values of all supply
centers within the nation.  This display is useful to get a feel for
where materials will be dispersed after production.
.IT
(b)lank -- No information is to be shown in this display position.
.IT
(K)eep -- This option indicates that the current display style in this
position should not be changed.  If this is encountered in the actual
display, it behaves like the blank display.
.UT
.CE
.CS "List of Highlight Styles"
.IT
(N)one -- Disable highlighting.
.IT
(a)llies -- Highlight sectors owned by nations friendly to the current
nation.
.IT
(d)esignation -- Highlight any sectors containing a specific major
designation.
.IT
(e)nemies -- Highlight those sectors owned by nations unfriendly to
the current nation.
.IT
(m)inor desgs -- Highlight any sectors containing a specific minor
designation (construction).
.IT
(n)eutrals -- Highlight those sectors which are neutral to the current
nation.
.IT
(o)wned -- Highlight those sectors owned by a specific nation.
.IT
(r)ange -- Highlight those sectors within supply range of a supply
center in the current sector.
.IT
(s)couts -- Highlight sectors which contain scouting units.
.IT
(u)nits -- Highlight sectors containing armies, navies, or caravans.
.IT
(y)our units -- Highlight those sectors containing armies, navies or
caravans owned by the current nation.
.IT
(M)ovable -- Highlight sectors which contain units which have movement
ability remaining.
.IT
(R)egion -- Highlight owned sectors within the range of influence of
the supply center in the current sector.
.IT
(S)upported -- Highlight those sectors, owned by the current nation,
which are within support range of any of their supply centers.
.IT
(T)radegoods -- Highlight those sectors whose tradegoods are of a
specific tradegood class.
.IT
(U)nsupported -- Highlight those sectors which are owned by the
current nation, yet are beyond the support range of their supply
centers.
.IT
(K)eep -- Do not change the highlighting when this display mode is
selected.  If this style is encountered in the actual display, it
behaves as if the "None" highlighting style is set.
.UT
.CE
.CS "The Normal Conquer Display Mode"
.PP
As you have probably noticed, the normal Conquer display shows a map
of the world in most of the screen area.  This map will contain one or
more character representations of the various sectors which are
visible to your nation.  The position of the characters gives an
indication of the relative location of the sectors on the map.
Sectors towards the top of the map are north of the sectors towards
the bottom, and sectors to the left of the map are west of the sectors
on the right of the map.
.PP
The current sector is indicated by the position of the cursor.  More
information about the current sector, such as who owns it, what the
actual coordinates of the sector are, and what, if any, of your troops
are within the sector, will be shown along the right-hand column of
the display.  You can see an example of such a screen below:
.SB
                                                           *Wzrd 1 x=250 p=10
                                                             m:100 sp:= st:Dfd
                         __                                >Mage 2 x=50 p=5
                      __/ -\e__                               m:100 sp:= st:Dfd
                   __/ %\e__/ %\e__                          >Mage 3 x=51 p=5
                __/ -\e__/ -\e__/ -\e__                         m:100 sp:= st:Dfd
               / %\e__/ -\e__/ %\e__/ -\e                      >Mage 4 x=50 p=5
               \e__/ %\e__/ -\e__/ -\e__/                        m:100 sp:= st:Dfd
               / %\e__/ -\eF_/ %\e__/ -\e                      >Mage 5 x=51 p=5
               \e__/ -\et_/ -\eF_/ -\e__/                        m:100 sp:= st:Dfd
               / %\e__/ -\eC_/ %\e__/ -\e                        --- more ---
               \e__/ %\eF_/ #\eF_/ %\e__/                       sector [0,0]
               / ^\e__/ %\e__/ -\e__/ %\e                       owner: Aereol
               \e__/ -\e__/ -\e__/ ^\e__/                       Capital    Elf
                  \e__/ %\e__/ -\e__/                          Forest     Flat
                     \e__/  \e__/                             pop: 16628 move:  1
                                                            food:    8 wood:  8
                                                            item: drama (c)
.SE
.PP
If you possess units within the current sector, information about each
of the individual troops will be shown, with two lines of information
for each unit.  For armies, the information will contain the type of
unit, the id number of the unit, the size of the unit, and the status
and movement capabilities of the unit.  For navies, the id number of
the fleet, the number of each class of ship within the fleet, and the
status and movement capabilities of the unit will be shown.  And
finally, the caravan units will have the size, id number, status and
movement capabilities of the units shown.  The currently selected unit
within the sector will have a marker other than the `>' sign next to
it.  Usually this will be a `*', but a `+' is used to indicate the
leader of a group of army units or a ship or caravan which is carrying
items onboard.
.PP
The above map shows all of the world which is visible to a nation
"Aereol".  The currently selected sector is the capital located at
sector [0,0] which is surrounded by a number of farming sectors.
There are a number of troops within the sector, including the nation
leaders and Ruler, the "Wizard", which is currently selected.
.CE
.CS "Zoooooom..."
.PP
Conquer has support for both hexmap and rectangular coordinate
systems.  There are also three levels of "zooming" available, where
clarity of information will be traded off with the number of sectors
visible as the scale increases.  Below are examples of each level of
zooming, with the hexmap coordinate system on the left and the
rectangular coordinate system on the right.
.SB
             ~ % % % ~ - F b ~ -              %-~~~~~~~--%%-j%,,
              % - - ~ ~ F F % -               ~~~~~~~~~%-FFF%jb,
             , , % % ~ ~ C b % %              ~~~~~~~~~-%~CFi%j-
              ~ i ~ ~ ~ ~ F % i               b~~~~%~s~--%FF,ii%
             % , ~ ~ ~ ~ ~ - ~ ,              ~~~~~~~~~%-%-,^--j
.SE
.PP
At the "full" zoom setting, only one display "slot" is available, but
many sectors are visible at once. It should be noted, though, that on
the hexmap, it is not possible to travel directly east-west, and that
sectors are, however, connected with those directly to their north or
south.  The rectangular system gains a much more compact display
because of the rectangular orientation of the screen.
.SB
         %   ~   -   F   b   ~   -          ~ ~ ~ ~ ~ - - % % - j
           ~   ~   F   F   %   -            ~ ~ ~ ~ ~ % - F F F %
         %   ~   ~   C   b   %   %          ~ ~ ~ ~ ~ - % ~ C F i
           ~   ~   ~   F   %   i            % ~ s ~ - - % F F , i
         ~   ~   ~   ~   -   ~   ,          ~ ~ ~ ~ % - % - , ^ -
.SE
.PP
At the "medium" zoom setting, less sectors are shown, but the sector
positioning is more apparent for the hexmap.  For your reference, the
"designation" display style is being used here, with the vegetation and
elevations of the sectors being shown when there is no designation
present.
.SB
       / %\e__/~~\e~~/ -\e__/ %\e__/ ^\e       |~~|~~|~~| %| -| ^| ^| %| %|
       \e__/~~\e~~/~~\e__/ %\eF_/ %\e__/       |~~|~~|~~|__|__|F_|F_|F_|__|
       / %\e~~/~~\e~~/~~\eF_/ -\eF_/ ^\e       |~~|~~|~~| -| %|~~| %| ^| #|
       \e__/~~\e~~/~~\e~~/~~\eC_/ %\e__/       |__|~~|__|__|__|__|C_|F_|__|
       /~~\e~~/~~\e~~/~~\e~~/~~\eF_/ -\e       | ^|~~| -| -| %| ^| %| ^| ^|
       \e~~/~~\e~~/~~\e~~/~~\e~~/ -\e__/       |s_|~~|__|__|__|F_|F_|__|__|
       /~~\e~~/~~\e~~/~~\e~~/~~\e__/ -\e       |~~|~~| %| -| %| -| %| ^| -|
       \e~~/~~\e~~/~~\e~~/~~\e~~/ -\e__/       |~~|~~|__|__|__|__|__|__|__|
.SE
.PP
Finally, the "detail" zoom shows more information for each sector and
demonstrates very clearly the directional relationship between
neighboring sectors.  The important thing to note here is that there
are now four characters of information for each sector.
.CE
.CS "The Information Mode"
.PP
When it is desirable to obtain more information about the current
sector than the standard conquer display provides, the information
mode is what you are looking for.  To enter the information mode,
simply use the "toggle-infomode" command, and it will switch between
the standard conquer display and the information mode.
.PP
The information mode is very much like the standard conquer display,
but the map takes up much less of the screen, and the extra space is
filled by a section which will show as much information about the
sector as the nation knows.  The right side of the screen is then
totally devoted to the listing of the troops within the sector.
.PP
An example of what might be shown in the information mode is:
.SB
       __/ %\e__/ %\e__         Capital of aereol [128]      *Wzrd 1 x=250 p=10
    __/ %\e__/ %\e__/ %\e__                                     m:100 sp:= st:Dfd
   / ^\e__/ %\e__/ -\e__/ %\e     Attract: 151   Constructions >Mage 2 x=50 p=5
   \e__/ %\e__/ ^\e__/ -\e__/     Weights: 128   fortified       m:100 sp:= st:Dfd
   / ^\e__/ ^\e__/ %\e__/ ^\e     Defense: 60%                 >Mage 3 x=50 p=5
   \e__/ %\eF_/ -\eF_/ ^\e__/     Recruits: 8000                 m:100 sp:= st:Dfd
   / ^\e__/ ^\eC_/ -\e__/ #\e     Range: 3.0                   >Mage 4 x=50 p=5
   \e__/ %\eF_/ v\eF_/ -\e__/     Talons: 200000                 m:100 sp:= st:Dfd
   / ^\e__/ %\eF_/ %\e__/ ^\e     Jewels: 15000                >Mage 5 x=50 p=5
   \e__/ %\e__/ -\e__/ %\e__/     Metals: 15000                  m:100 sp:= st:Dfd
      \e__/ %\e__/ ^\e__/        Food: 40000                  >Mage 6 x=50 p=5
         \e__/ ^\e__/           Wood: 15000                    m:100 sp:= st:Dfd
            \e__/                                           >Mage 7 x=50 p=5
                                      Produce   Consume      m:100 sp:= st:Dfd
          sector [0,0]        Talons  154011    7500       >army 200: 175 inf
owner: aereol                 Jewels  0         0            2 m:0(-) st:Grsn
Forest    Flat      Capital   Metals  0         860        >army 201: 75 inf
people: 23335       Elf       Food    8         23335        2 m:100(=) st:Dfd
food: 8   wood: 8   move: 1   Wood    8         2620       >army 202: 75 inf
item: drama (c)                                              2 m:100(=) st:Dfd
.SE
.PP
The display shown above concerns the Capital of the nation aereol.
The left portion of the screen is devoted to the map of sectors around
the capital, the right column to the troops within the capital area,
and the middle of the screen shows a good deal of information about
the supply center itself.
.PP
At the top of the information portion of the screen, the sector
designation will be shown, with the value of the designation shown in
parenthesis.  For supply centers, this will be the weight assigned to
the sector; for farms, it will be the food value; for metal mines, the
metal value; etc.
.PP
Below that will be listed all of the constructions, a.k.a. minor
designations, as well as the attractiveness of the sector to the
civilians.  Immediately below the attractiveness is the sum of the
weights of all supply centers within support range of the sector.  [In
the above display, this shows that the capital is the only supply
center within range]  Below that is the defensive value of
fortifications within the sector.  At the very bottom, the production
and consumption summary is given, so that you can determine just where
all of the national resources are going.
.PP
If the sector is a supply center, such as the capital above, there
will be some added information, such as how many volunteers are in the
sector, what the influence range of the supply center is, and what
materials are stored within the supply center.  Otherwise, the number
of civilians actually working within the sector will be shown.
.RF "Sector" "Sector Information"
.CE
.CS "The customize-display Command"
.PP
As stated earlier, a display mode consists of a specification for all
four character slots, highlight specifications, and a focus.  There
are, therefore, many possible combinations of display modes available.
The command "customize-display" is used to create a specific
combination, and perhaps give a name to it, so that it can be put to
use at a later date.
.PP
When first executed, the list of currently defined display modes is
listed.  Simply specify the name of the display mode you wish to edit,
and press return.  If the display mode is not already defined, a new
display mode will be created under that name.
.PP
The bottom portion of the screen should look something like:
.SB
 Name: "Standard"      Display and Highlighting Selections               __
 Focus: low-left         blank / owned        | contour / owned         / -\e
  '?' for commands       ---------------------+---------------------    \eC_/
  'q' to exit            your desg / owned    | blank / owned
.SE
.PP
The name of the mode being edited, the current focus position and the
two brief command selections are listed on the left.  The settings for
each position (or slot) are listed in the middle.  And a view of the
current sector using the display mode under construction is on the
right.
.PP
So, you use the space to change the focus of the display mode, then
use the `d' key to change the display style for a given position.  You
can also change the highlighting using the `h' key, or any of the
highlighting keys available in the normal conquer mode, and the name
using the `n' key. If you wish to return the settings to the original
values, the `r' key will perform a reset.  All of these keybindings
are mentioned when the `?' key is pressed, if you should forget.
.PP
Finally, when you leave the display customizer, using the `q' key, it
will store your newly created display mode in the list of defined
display modes.  It will then be accessible via the "adjust-display"
command.  If you specified to edit the current display mode, conquer
will ask you if you wish to try to save the settings under some new
name so that it will not be lost when you switch to another display
mode.  An important bit of information here:  the position of the
focus when leaving the customizer is the value of the focus in the
actual display mode.
.PP
Be aware, though, that the customized display modes only remain in
existence for the duration of the current conquer session.  You may
have conquer store the settings, though, using the "conquer-options"
command and its "store-settings" subcommand.
.CE
.CS "Predefined Display Modes"
.PP
Conquer comes with a default list of display modes, so that players
need not design their own unless they wish to.  The list of predefined
modes is:
.TB
       name      low-left  | low-right |  up-left   | up-right  |  focus
 =============  ===========|===========|============|===========|===========
 Standard       designation|   blank   |   blank    |  contour  | low-left
 Designation    designation|   keep    |   keep     |   keep    | low-left
 Own Designati   own desg  |   keep    |   keep     |   keep    | low-left
 Nation Marks      keep    |   keep    |nation mark |   keep    | up-left
 Race              keep    |   keep    |   race     |   keep    | up-left
 Population        keep    |  people   |   keep     |   keep    | low-right
 Contour           keep    |   keep    |   keep     |  contour  | up-right
 Vegetation        keep    |   keep    |   keep     |vegetation | up-right
 Tradegoods        keep    |Tgood desg |   keep     |   keep    | low-right
 Jewels            keep    |   keep    |  jewels    |   keep    | up-left
 Magics            keep    |   keep    |Magic value |   keep    | up-left
 Metals            keep    |   keep    |  metals    |   keep    | up-left
 Values            keep    |   keep    |  Values    |   keep    | up-left
 Food              keep    |   keep    |   food     |   keep    | up-left
 Wood              keep    |   keep    |   wood     |   keep    | up-left
 Defense           keep    |   keep    |  Defense   |   keep    | low-left
 Army Move         keep    | Army Move |   keep     |   keep    | low-right
 Navy Move         keep    | Navy Move |   keep     |   keep    | low-right
 Flight Move       keep    |Flight Move|   keep     |   keep    | low-right
 Weights           keep    |  Weights  |   keep     |   keep    | low-right
 Blank             blank   |   blank   |   blank    |   blank   | low-left
.TE
.PP
Note that all of the default display modes have a highlight style of
"Keep" to assure that they will not alter the current highlighting
method.
.CE
