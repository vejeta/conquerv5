.CH "Unit Relocation"
.CS "The Concept of Motion"
.PP
Within conquer, there are three types of mobile units:  armies,
navies, and caravans.  Armies and caravans will generally travel over
land, while the navies will roam the oceans.  Flight is also possible
for those army units which naturally possess flight, or which are
given the property of flight via magic.  Using flight, army units are
able to traverse many places, such as oceans, which would otherwise be
inaccessible.
.PP
The "Movement" attribute of a nation is a quantitative indication of
how well orders are distributed to military units, as well as a rating
for how quickly a unit may be relocated.  This, combined with the
inherent quickness of the unit, will give an indication as to how far
a unit may move in a single update.  This quantitative value can be
thought of as movement points, and is displayed in the extended
information portion of the "command-unit" function.
.PP
Sectors will each have a relative movement cost, based on the terrain
and vegetation of the sector, as well as the racial characteristics of
the nation.  These costs are in the same movement points as the
"Movement" attribute of a nation and the movement potential of a unit.
.PP
Each unit also possesses a speed, which indicates the rate of motion
of the unit.  The faster a unit is moving, the more movement potential
a unit will possess.  But, there is a trade-off, because a unit moving
faster is much more vulnerable to adverse conditions and combat than
units which are moving slowly due to the energy expended during
motion.
.PP
It is possible to view the various movement costs for each sector
using the display routines.
.RF "Display" "Conquer Data Display Routines"
.PP
If you find the concept of movement points confusing, you really
shouldn't worry about it, because the program will calculate all of
the movement in percentages.  The movement points need only be taken
into account if you wish to compute the relative movement costs for
sectors or the relative speed of units.
.PP
It should be noted that the diplomatic status between two nations can
determine whether or not a sector is accessible to a nation.
.RF "Nations" "Racial Traits and Nation Roles"
.CE
.CS "The Commands Governing Motion"
.PP
There are a number of commands which affect the motion of units.  The
most obvious command is "move-unit" which is used to actually relocate
a unit.  While in the main Conquer map, simply highlight the unit you
wish to move, and perform the "move-unit" command, which is bound to
`m' key by default.
.PP
Another command which will affect the movement ability of a unit, is
the "command-unit" function, which is bound to the Escape key by
default.  This command can adjust the status of the unit, which
determines whether or not a unit will actually be able to move.  Of
even more importance, are the speed adjustments within the
"command-unit" function.  These are used to directly adjust the unit
speed.
.RF "Warfare" "Military Units and Combat"
.PP
There are three possible unit speeds: "Slow", where movement potential
is half of the normal; "Normal", with movement potential being met;
and "Fast" with time and a half the movement potential otherwise
possible.  The drawback to the various speeds is that troops may die
from harsh travel at the faster speeds, and their combat bonus is
greatly affected by the varying speeds, as the chart below
demonstrates:
.TB
    Speed Setting              Army Bonus     Caravan Bonus    Navy Bonus*
    =================         ============    =============   ============
    Slow                          +40             +20             -40
    Normal                          0               0               0
    Fast                          -40             -20             +40
.sp
        Armies and Caravans are less combat ready if moving faster,
        while speed on the seas can only help a naval unit in combat.
.sp
      *Navies in a land sector have no speed adjustment within combat.
.PP
Also, if the unit is flying, that unit will have an additional 20
combat bonus points added on.
.RF "Warfare" "Military Units and Combat"
.TE
.CE
.CS "Army Motion"
.PP
Army units can be relocated in a number of different ways.  They can
move on their own from one sector to another.  They can be carried
over water using naval warships.  They can fly after having the spell
of "Flight" cast on them, thus being able to cross water sectors
without a navy.  And, they can be grouped together under a leader, and
moved when that leader moves.
.PP
The movement potential for an army unit is directly proportional to
the national "Movement" attribute.  This value will be multiplied by
the movement rate of the unit type, and then adjusted by the speed of
the unit.
.PP
While moving, army units, if not flying or scouting, can be stopped
whenever they encounter a group of hostile troops or a wall sector
owned by a hostile nation.  Any garrisoned unit large enough to hold a
land sector will be able to stop any sized army unit.  If the
garrisoned units are at least half the strength of the units trying to
move, then those units will only be able to leave the sector by going
the same way out as they used to come in.  Non-garrisoned troops are
considered to be mobile within the sector, and as such are not
positioned properly to prevent enemy movement.
.PP
There is one other form of movement available to army units:  that of
being instantly transported from one sector to another.  If the nation
possess the proper magical knowledge, and they have a wizard with
enough power, they can instantly relocate a unit from one sector to
another using the spell of "Teleport".
.RF "Powers" "Magical Powers"
.CE
.CS "Caravan Motion"
.PP
Caravan units move in mostly the same manner as army units, except
the heavy wagons are not too prone towards either flight or
teleportation.  They will suffer the same prevention of motion as army
units within sectors controlled by others, and will also have their
movement ability governed by the "movement ability" of the nation.
.PP
The exact number of movement points which a caravan receives is half
that of the national movement ability (with a minimum value of 6),
adjusted by the speed of the caravan.  If any crew is missing from a
caravan, the caravan will lose the same percentage of mobility.
.PP
Caravans are useful for conveying both civilians and materials around,
but they are not able to be moved across water sectors at all.  This,
combined with their minimal combat effectiveness means that caravans
while useful in moving materials to sectors that need them, are rather
vulnerable to attacks if not properly protected.
.CE
.CS "Navy Motion"
.PP
Naval fleets can prove to be a powerful weapon for your nation.  Their
large capacity for storage and ability to traverse water sectors gives
them the potential to make or break the economy and/or military of
your nation.  Whenever a fleet is relocated, all the materials,
civilians, and troops onboard are carried along with it.
.PP
The movement points which a fleet may possess are dependent on the
type of ships within the fleet.  Light ships have 3 more movement
points than medium ships, and 6 more than heavy ships.  Warships have
a base rate of 30 (for heavy models).  Galleys have a base rate of 25
and the slower Merchants have a base rate of 20.  Finally, the very
slow Barges have a base rate of 10. The movement rate of the entire
fleet (or unit) is the movement rate of the slowest ship in that
fleet.  If the fleet is missing any crew, the percentage of missing
crew will indicate the percentage of lost speed.
.PP
If a navy unit is within a land sector, that unit is considered to be
beached, or in the harbor, if the sector possesses one.  Once in a
land sector the navy can load or unload armies using the "transport"
command after the fleet is highlighted as a selection.  It should be
noted that only those troops which are experienced at making
beachheads, such as Sailors or Marines, may disembark within territory
not owned by the nation.  If troops are lead off by a leader, however,
the restriction concerning unowned territory will be waived.  If the
land sector in question contains a supply center owned by the nation, 
raw materials may be transferred between the two.
.RF "Economics" "Economic Management"
.PP
In order to supply units using any materials onboard, the naval unit
must be either on land, or at sea, matching the type of sector which
the unit to receive supplies is within.
.CE
.CS "The Movement Process"
.PP
The actual method of moving a unit from one sector to another is
fairly simple.  You select the unit that you wish to move by
highlighting it using the "pick-previous" and "pick-next" commands,
and then execute the "move-unit" command.
.PP
If the unit is able to be moved, you will will be now be in the
movement-mode of conquer, otherwise an error message showing why the
unit was unable to be moved will be displayed.  Once within the
movement-mode, you are able to relocate the unit by telling it which
direction to go using the proper directional command.
.PP
The percentage of movement available to the unit will be displayed,
and the percentage costs for relocating to each of the neighboring
sectors will be shown in the bottom portion of the screen.
.PP
If the sector in the direction you chose is a valid one for the unit
to move into, you will be switched to that sector.  Otherwise, the
reason for your not being able to enter the sector will be given, and
the movement will be disallowed.  You can then enter another direction
to move towards, or hit the space bar to stop your movement.
.PP
As the unit moves through each sector, the percentage movement cost of
the sector is deducted from the unit's movement potential.  If the
unit runs out of movement or if you end the movement phase, the motion
will be stopped and the unit will now be located in the current
sector.  Stopping motion will cost an additional 5% of movement
potential, so you should make sure to take this fact into
consideration if you wish to move the unit again during the same turn.
.PP
Once the final sector has been reached, the unit will be relocated to
that sector.  If the unit was flying and "lands" in a sector that is
incompatible with the unit's survival, the unit will be destroyed.  At
this point, if the unit is flying via magical means, the power of
flight will be lost, with the flight spell being broken upon contact
with the ground.
.CE
.CS "The Other Movement Methods"
.PP
Besides the normal, cost involved method of motion, you can relocate
things using a sector selector.  Relocating people using the
"move-people" command, or transporting an army unit using the
"Teleport" spell are both examples of this technique.
.PP
With such forms of movement, the distance of the movement is what
matters, not the actual relocation route itself.  In other words, you
can select any sector within a given range, as long as you can get to
that sector.  This sort of movement can be aborted using the
"move-quit" command, which is usually bound to the Escape key.
.CE
.CS "The Costs for Land Movement"
.PP
For each sector, there is a given movement cost to enter that sector.
This cost is determined by the race of the nation doing the entry, the
vegetation of the sector, and the elevation of the sector.  The
movement cost for a sector can also be halved through the use of the
minor designation of roads.
.PP
The list of the land movement costs for each of the elevations, by
race, is:
.TB
       Elevation Level         Orc     Elf     Dwarf   Human   Others
      ===================     ======= ======= ======= ======= ========
       Water (*)                ---     ---     ---     ---     ---
       Valley                     2       3       2       3       3
       Flat                       1       1       1       1       1
       Hill                       2       2       1       2       2
       Mountain                   3       5       3       4       4
       Peak                       4       8       4       6       6
.sp
        (*)  - Water sectors may be made passable using bridges,
               causing such sectors to possess a movement cost of 2.
.TE
.PP
The added land movement costs for each vegetation type, by race, is:
.TB
       Vegetation Type         Orc     Elf     Dwarf   Human   Others
      ===================     ======= ======= ======= ======= ========
       Volcano                  ---     ---     ---     ---     ---
       Desert (*)                 3       6       5       3       3
       Tundra                     2       4       3       3       3
       Barren                     1       1       0       1       1
       Light Vegetation           0       1       0       0       0
       Good                       0       0       0       0       0
       Wood                       2       0       1       0       0
       Forest                     2       0       3       1       1
       Jungle (**)                4       2       5       3       3
       Swamp (**)                 3       2       4       3       3
       Ice (*)                    7       7       7       2       2
       None                     ---     ---     ---     ---     ---
.sp
        (*)  - Possessing either the magic power of Dervish or
               Destroyer reduces this cost to 1.
        (**) - The magic power of Amphibian reduces this cost to 1
.TE
.PP
Basically, you add the movement cost for the elevation of the current
sector with the movement cost for the vegetation and you obtain the
movement cost for the entire sector.
.CE
.CS "The Costs for Naval Movement"
.PP
The method used to determine the cost of movement for water sectors is
fairly straight-forward.  It costs all of the movement points for a
ship to enter a land sector which does not possess a canal or a
harbor.  Movement in water sectors is easier along coastal regions,
and more difficult in deep water.
.PP
For navies, land sectors come in three forms:  "tera" - inaccessible
land sectors; "costa" - land sectors neighboring water; "waterways"
- land sectors with harbors or canals for smooth naval access.  The
"tera" sectors cannot be reached via ships.  The "costa" sectors can
be moved into, but the navy losses all movement by beaching the craft
in this manner.  And the "waterway" sectors have a movement cost
of either two or four, depending on whether the sector is a canal or a
harbor respectively.
.PP
Finally, the movement cost for a water sector is dependent on the
number of neighboring water and land sectors.  If the sector is
totally surrounded by land or water, the cost is 5.  If there is an
even balance of land and water sectors, the cost is 1.  The rest of
the possibilities range between those two extremes.  This creates
movement costs such that travel along open coastal regions is much
easier than over open ocean.
.CE
.CS "The Costs for Flight"
.PP
Flight, naturally available to a number of army unit types, can make
pinning down a unit almost impossible.  Their ability to traverse
blockades and fly over water make them swift scouts and deadly strike
forces.
.PP
The movement costs while flying are greatly reduced from the normal
land movement, and are independent of the national race.  The costs,
specified by vegetation and elevation, are:
.TB
       Elevation Level         Cost       Vegetation Type         Cost
      ===================     ======     ===================     ======
       Water                     1        Volcano                   4
       Valley                    3        Desert                    1
       Flat                      1        Tundra                    1
       Hill                      1        Barren                    0
       Mountain                  2        Light Vegetation          0
       Peak                      6        Good                      0
                                          Wood                      0
                                          Forest                    0
                                          Jungle                    1
                                          Swamp                     0
                                          Ice                       0
                                          None                      0
.TE
.CE
.CS "Commands within movement-mode"
.PP
Once within movement-mode, there are a number of commands available to
the user.  Most of these commands simply allow for the motion of the
unit, but some other commands are also available.  The full list of
commands is:
.IT
adjust-display (`d') -- This command is the same as the "adjust-display"
command within the normal conquer mode.
.IT
conquer-options (`O') -- This command is also similar to the normal
"conquer-options" commands, but is available here to allow the
rebinding of movement-mode keys.
.IT
customize-display (`D') -- Another display command to adjust the
viewing of the map.
.IT
highlight-all (`=') -- Change all of the highlighting slots.
.IT
highlight-current (`,') -- Change the highlight at the focus slot.
.IT
highlight-horizontal (`-') -- Change the highlight of the focus and
neighboring slot.
.IT
highlight-upleft-lowright (`\e') -- Change the highlight of the upper
left and lower right slots.
.IT
highlight-upright-lowleft (`/') -- Change the highlight of the upper
right and lower left slots.
.IT
highlight-vertical (`|') -- Change the highlight of the current slot
and the slot above or below.
.IT
ignore-key (`5') -- Bind this to any key that you wish to be able to
hit, but not have do anything.
.IT
move-east (`6',`l') -- Move your unit one sector to the east.  With a
hexmap world, this function is useless.
.IT
move-exit (` ',`q') -- Indicate that your movement is complete, and
have the unit placed in the current sector.
.IT
move-help (`?') -- Display this list of functions.
.IT
move-north (`8',`k') -- Move the unit one sector to the north.
.IT
move-northeast (`9',`u') -- Move the unit one sector to the
northeast.
.IT
move-northwest (`7',`y') -- Move the unit one sector to the
northwest.
.IT
move-quit (ESC,`Q') -- Attempt to abort the current movement.  This
is usually equivalent to the "move-exit" function, but when movement
people or teleporting a unit will abort the operation.
.IT
move-south (`2',`j') -- Move the unit one sector to the south.
.IT
move-southeast (`3',`n') -- Move the unit one sector to the southeast.
.IT
move-southwest (`1',`b') -- Move the unit one sector to the southwest.
.IT
move-west (`4',`h') -- Move the unit one sector to the west.  In a
hexmap world, this command has no use.
.IT
recenter-map (`^R') -- Relocate the current sector to the center of
the screen.
.IT
shift-focus (`+',`)') -- Shift the focus slot to the next position.
.IT
shift-focus-back (`(') -- Shift the focus slot to the previous
position.
.IT
redraw-screen (`^L') -- Clear and redraw the screen.
.IT
toggle-infomode (`@') -- Switch into and out of the information mode
of the map display.
.IT
troop-listing (TAB) -- Show any troops not controlled by your
nation, which are visible in the current sector.
.UT
.CE
