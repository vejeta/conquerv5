.CH "Conquer God Mode"
.CS "The Power of God"
.PP
A world in Conquer is a complex thing, and needs to have someone to
watch over it.  The creation of the world, the updating of the turns,
and the monitoring of events are all assigned to the control of one
person.  It is that person's responsibility to make sure that the game
flows smoothly and fairly.  The title of "god" is given to this
person, and any aspect of the game may be adjusted by them.
.PP
There are times when players will be unable to get their nations
started properly, such as if there are no metal or wood sectors near
them, or if, though pure bad luck, they are placed on a very small
island.  Or, the nation might accidently lose all of their money by
drafting the wrong type of unit by accident.  In such situations, god
may add land or resources to the nation, disband troops and return
spent resources, or even give gifts of wood or metals to the nation to
provide them with a second chance.
.PP
If the site is large enough, or the tasks of the god too time
consuming, it is possible for the god to give another player the
ability to be a "god" to a world.  Such a person would be called a
"demi-god".  In this way, many campaigns may take place on a single
machine, with different demi-gods monitoring each.  This provides the
ideal environment for players, where they can have a variety of
worlds, each one configured just a bit differently.
.CE
.CS "Genesis:  A Conquer Cookbook"
.PP
To begin a Conquer game, there must first be a world upon which to run
the campaign.  A Conquer world is created using the command:
.SB
        conqrun -m -d campaign
.SE
.PP
If an empty string is specified [conqrun -m -d ""], then the default
Conquer campaign is assumed.  If no '-d' flag is used, then either the
default campaign is used, or the campaign specified using the
CONQ_OPTS environment variable is used.
.RF "Customize" "Conquer Customization Settings"
.PP
Once a check is made to be sure that the world creation is both
allowed and desired, a startup screen is displayed.  After a key
press, Conquer will prompt you to enter super-user password for the
new world.  This password will be used whenever access to god powers
is needed, to preserve security.  Once that has been entered, the
configuration screen will be presented to you.
.CE
.CS "The Configuration Screen"
.PP
The configuration screen is a mode where the world parameters can be
specified, and where different options can be enabled or disabled.
The screen itself will look something like:
.SB
                         < Conquer Configuration Mode >
.sp
 Demi-god..highlndr  Min Ntn Dist....13  Check Range......2  Scout Battles..25%
 God Passwd........  Navy Bonus.....-25  Pct Reject....100%  Rand Events......6
 Add Passwd........  Cvan Bonus.....-50  Pct Tgoods.....90%  Disasters........5
 Limit D-god..False  Combat Dice......9   % Metals......25%  Mercenaries..91171
 D-god Builds..True  Avg Damage.....50%   % Jewels......25%   Atck Bonus....+75
 Map Type....HexMap  Deter Damage...50%   % Magical.....15%   Dfns Bonus....+75
 Relative Map..True  Overmatch......10%  Growth Rate....1.0  % Disb Mercs...15%
 Verify Login..True  Min Damage......5%  Npc Ntns.........4  % Mnst Mercs....6%
 Hide Scores...True  Year.............8  Monst Repro....20%  Max Diplo Adj....1
 Hide Users....True  Month.......Junius  Lizards.........50  Bribe Value.200000
 Late Start.......5  Pct Water......65%  Savages.........50  Max Supply.......3
 World X Size...136  Pct Contour....75%  Nomads..........50  Unit Sply Dist...1
 World Y Size...136  Expose Ratio...2.0  Pirates.........20  City Xfer Dist...2
 Max Build Pts..100  Smoothings.......7  Revolt Pct.....25%
.sp 3
 Conquer Version 5.0: World Adjustor                        Datadir: [default]
-------------------------------------------------------------------------------
  'hjkl'-select item  ' ',DEL-cycle  'c',ESC-change item  'q'-save   'Q'-quit
.sp
.SE
.PP
If your terminal supports it, the current item will be highlighted.
In any case, the cursor will be placed after the current item.  If the
new world is being created over an old one, the configuration of the
prior world will be kept, otherwise the default configuration will be
used.
.PP
To change a configuration option, use the VI movement keys, or the
numeric keypad, to move to the selection you wish to change, and hit
the `c' or ESCAPE key.  Then enter a new value.  When you are done
specifying the world parameters, enter a `q' if you wish to build the
new world or a `Q' if you wish to abort the creation.
.CE
.CS "Configuration Options"
.PP
In order to clarify what each of the possible configuration options
is, I have provided a description of each of them.  Those with an
asterisk (*) following the name are adjustable at any time after the
world has been built:
.IT
Demi-god* -- During compilation, the user who is empowered to control
all of the Conquer campaigns is specified using the LOGIN definition
in the header.h file.  While it would be nice if a single person could
manage all of the campaigns that a site would want to run, experience
has shown that only with large amounts of caffeine would it be close
to possible.
.ST
Thus, the demi-god is born!  The demi-god is the login name of the
person that god designates to run that particular world.  The powers
of the demi-god may be limited to just world updating, or can include
all of the powers that god possesses but only for that particular
world.
.ST
Whether or not a world has been assigned a demi-god, the powers, if
not the responsibilities, of god will remain the same.
.ST
The string "[none]" may be used to specify that there should be no
demi-god for a campaign.
.IT
God Passwd* -- In order to access all of the powers of god, a password
will need to be entered.  This option allows the adjustment of the
password.  Demi-gods must know the old password before changing it.
.IT
Add Passwd* -- After a number of game turns have passed, users will be
asked to enter a password before being allowed to create a new nation.
This option controls the setting of that password.  Demi-gods must
know the old password before being allowed to change it.
.IT
Limited D-god* -- If set, this option will prevent the demi-god from
being able to enter the nation "god", and will also prevent the
demi-god from being able to rebuild the world.  In essence, this means
that the demi-god will only be able to update the game.
.IT
D-god builds* -- If this option is set to "False", the person
designated as the demi-god will not be able to rebuild the world and
start a new game.  This is useful for preventing people from starting
a new campaign without checking with god first.
.IT
Map Type -- This is a nice, nifty, neato-keen, feature which allows
you to determine what map will be used for a campaign.  The two
possible choices are HexMap and RectMap.
.ST
The RectMap is the plain old 8 directional coordinate system.  The
HexMap, on the other hand, is where each sector has only 6 neighboring
sectors, and all sectors are joined by the edges.  The Hexagonal
coordinate system is a much more realistic system and should be the
preferred system, but the RectMap is provided as an alternative for
those who wish to use it.
.IT
Relative Map* -- If this option is set to "True", everyone, except
god, will have sector locations displayed as the relative displacement
between the current location and some central sector [By default, the
national capital.]  So, the sector one up and to the left of the
capital will appear to be [-1,1] to that nation.
.ST
The reason this feature was added was to decrease the amount of help
player nations can give each other if they have not actually met.  [I
can't put limitations on the mail system, since they can just use the
normal computer mail if they want... this feature serves as a nice
compromise.]
.IT
Verify Login* -- Each nation has, stored within it, the login name of
the person who created it.  If this option is set to "True", only that
person will be allowed access to the nation.  This acts as an added
security feature and will also enable a check preventing people from
creating more than one nation, attempting to thus gain an advantage by
doubling their information resources.  [I got sick and tired of having
to delete nations created by people who would create two or three
nations and then play out the one with the better starting position.
Annoying cheaters!]
.IT
Hide Scores* -- If this configuration option is set to "True", any
information which could possibly determine a national strength or
ability will be hidden from players during the various score reports.
.IT
Hide Users* -- If set to "True", the identity of the nation owners
will not be displayed during score reports to other nations.
.IT
Late Start* -- After a number of turns have been played in the game,
nation creation will not be allowed for all players.  This option
controls how long, in game turns, before users will be asked for the
Add Passwd when trying to build a new nation.
.IT
World X Size --
.NT
World Y Size -- These two options control the size of the world used
in the campaign.  The larger the world, the longer before nations
meet, and, thus, the longer the game.  When adjusting the size of the
world, be sure to also adjust the number of monsters and NPC nations
within the world.
.IT
Max Build Pts* -- When creating nations using the "conqrun -a"
interface, each of the options within the nation configuration is
assigned a "cost" which will be deducted from the construction points
remaining.  The "Max Build Pts" are the total number of construction
points which a nation will be given at the start of the nation
building process.
.IT
Min Ntn Dist* -- This setting is used to determine how far apart
nations' capitals must be when initially placed.  Be sure to reduce
this value if you are building a small world.
.IT
Navy Bonus* --
.NT
Cvan Bonus* -- During combat between the various military units, each
unit is assigned a combat bonus.  Army units are given a default bonus
of zero, which is then adjusted by the combat bonus of the particular
army type.  These two options control the adjustment of the combat
bonuses for navy and caravan units, and can have a great affect on the
results of combat for those units.
.IT
Combat Dice* -- When a military conflict occurs within Conquer,
Conquer will roll a number of combat dice to determine the outcome of
the battle.  With more dice, a more pronounced bell curve is given,
but more time is required to compute the results.  With less dice, the
bell curve becomes flatter and the results become much more random,
but the results are more quickly determined.
.ST
The "conqrun -Q" command is available to set and/or test the various
dice settings available.  For example, on my system, the results show
that using 9 dice is superior to using 10, since the algorithm used
ends up with a smaller standard deviation at that setting.
.IT
Avg Damage* -- During combat, there is a certain amount of damage
which will occur to each troop.  The "Avg Damage" is the damage that a
military unit will receive if it were to meet a unit of the same
combat strength and a balanced result takes place.  Increasing this
value makes combats more devastating to all units involved.
.IT
Deter Damage* -- Attacking forces will keep on searching for enemy
troops until they have engaged all enemy forces, or they have received
enough damage in one encounter to cause them to turn back.  This
selection controls what that amount of damage is.
.IT
Overmatch* -- When the number of troops is larger for one side in an
engagement than the other, the combat damage will be adjusted by this
percentage for each odds level of a combat.  For example, with an
Overmatch value of 10% and if side A is 3 times the size of side B,
side A's damage will be decreased by 20% and side B's will be
increased by 20%.  Increasing this value increases the affect of
numerical superiority.
.IT
Min Damage* -- This setting indicates the minimum amount of damage
which a unit must receive, based on the combat roll.  This is
calculated after the Overmatch to assure that a military unit will
suffer something for the battle.  So, if a unit would have received
25% damage, but by Overmatch it went below zero, the troop would get
an actual damage result of 5%x25% or 1.25% worth of damage.  This is
calculated based on a Min Damage value of 5%.
.IT
Year* --
.NT
Month* -- During world creation you may specify a different time at
which the game is to begin.  This is mostly provided as a cosmetic
feature, but by specifying a different month, you can control the
season during which the campaign will begin.  You might want to refer
to the calendar information, to get a clear idea of the dating scheme
used by Conquer.
.RF "Overview" "A General View of Conquer"
.IT
Pct Water -- This item is also fairly straight forward.  It controls
how much of the world should be composed of water sectors.  I
recommend at least fifty percent to make the use of naval fleets
worthwhile, even higher if you wish to require the use of such units.
.IT
Pct Contour -- This option controls what percentage of the sectors
which are land sectors should be non-flat in nature.  Non-flat means
any of: Valley, Hill, Mountains, or Mountain Peak.
.IT
Expose Ratio* -- This value controls how hazardous it is for troops to
be out in bad weather or vegetation.  Lowering the value makes it less
hazardous for troops.
.IT
Smoothings -- This item is the most important in determining the
general shape of the land masses in the world.  The number entered
under this option means literally what it says: the number of times a
smoothing algorithm is run on the world.
.ST
The smoothing algorithm is simple: on a N% (currently 33%) chance,
change the current sector to be land or water depending on whether
there is more water or land within a 2 sector radius.  The altitude of
the land sectors are also raised or lowered depending on if there are
more things nearby which are higher or lower.
.ST
Setting this number to 1 creates a very splotchy world with a large
number of small islands scattered around, and setting it to the
highest value (15), creates a world of solid land masses with no small
islands.  [Except for pirate bases which are ALWAYS single sector
mountainous islands]
.ST
I usually choose a value between 5 and 7 depending on how many little
islands I want to have scattered throughout the world.
.IT
Check Range --
.NT
Pct Reject -- These two options control the initial placement of the
solid 8x8 blocks of land.  All of the solid land masses are placed,
with surrounding blocks of "75% land, 25% water", at random.  A check
is made of blocks within "Check Range" to determine if there is
already land close by.  If there is, that location will be rejected on
a "Pct Reject" chance, and a new location will be selected.
.ST
A larger range and higher percentage causes more separate continental
islands to be placed, while lowering them will allow the possibility
for a single dominant land mass to be formed.
.IT
Pct Tgoods --
.NT
% Metals --
.NT
% Jewels --
.NT
% Magical -- These options control the amount of tradegoods placed
during the creation of the world.  The "Pct Tgoods" is the percent
chance that a land sector will contain some form of tradegood, and the
other percentages indicate what chance there is that a tradegood will
be of the given type.
.ST
Since there are types of tradegoods other than the metal, jewel, and
magical tradegoods, there is a limit to the combined percentage of
those options.
.IT
Growth Rate* -- The player nations, NPC nations and monster units
within Conquer each will slowly grow in size as time passes.  The
growth is controlled by the reproductive rates of each of the nations
and the "Monst Repro" and "% Mnst Mercs" settings for monsters.  It is
possible to globally increase or decrease the reproductive ability by
adjusting this value.  Increasing this value increases the rate at
which nations will grow.
.IT
Npc Ntns -- This controls the number of non-player character nations
that are created when the world is built.  These nations are read from
the "nations" data file, so it is also limited by the number of
entries within that file.  You may use the "conqrun -A" command to add
more nations to the "nations" file, and the "conqrun -Z" command to
remove all of the nations.
.IT
Monster Repro* -- This selection controls how quickly the monster
nation populations increase on a yearly basis.  It is offset by both
the death of the monster races (lizards, nomads, savages, pirates and
peasant rebellions) from combat, as well as setting of the "% Mnst
Mercs" option.
.IT
Lizards -- This indicates exactly how many lizard castles should be
placed upon the world.  Each lizard castle will be placed upon a
highly valuable jewel tradegood, and will have fortifications
consistent with the jewel level of the sector.  Two lizard army troops
will be guarding each castle: one roaming infantry troop, and one
garrisoned archer troop.  The combat bonus of the lizard troops is
that of the mercenaries during the build, plus 20 to the defensive
bonus.
.IT
Savages -- Units of this nation randomly roam about the world as
agressive army units.  Half of all of the savage troops are actually
monster units of some type, with the size an inverse ratio to the
strength of the troop, the remaining unit are randomly sized infantry
troops.  Combat bonuses for savages are those of mercenaries at the
time the world was built.
.IT
Nomads -- This nation is composed of a number of armies of Light
Cavalry.  They must move every turn, never staying in any sector for
more than a month.  The combat bonus of nomad armies is that of the
mercenaries at build time, plus 10 more to the attack bonus.
.IT
Pirates -- Controlling the seas, these pirate bases each have a fleet
of warships and an army of soldiers protecting its horde of minerals.
The combat bonus of the pirate nation is that of the mercenaries at
build time, with an adjustment of plus 20 to the attack, and minus 10
to the defense.
.IT
Revolt Pct* -- Every turn, each nation will have a certain chance that
the population will rebel and break away from the mother nation.  This
item is used as a multiplier of the calculated chance to influence the
chance of revolts.  Thus, a setting of 100% means no adjustment, while
a setting of 0% prevents revolts altogether.
.IT
Scout Capture* -- This indicates the chance that scouts may be dragged
into combat and possibly be destroyed.
.IT
Rand Events* -- On the average, how many random (non-weather) events
take place over the entire world during each update.
.IT
Disasters* -- How many random weather events should occur each update.
.IT
Mercenaries* --
.NT
Atck Bonus* --
.NT
Dfns Bonus* -- These options control the amount and strength of the
mercenary troops within the campaign.  It is also used as a base
determinant for the "monster" nations.  Note: Once a world is built,
God may change the mercenaries' combat bonus, but it will not affect
the combat bonuses of the various monster nations.  At that point, the
monster combat bonuses must be adjusted like that of any other nation.
.IT
% Disb Mercs -- This option determines what percentage of disbanded
troops decide to become mercenaries, instead of meekly becoming part
of the population.
.IT
% Mnst Mercs -- This option determines what percentage of men within
the monster nations decide to join the pool of available mercenaries
each year.
.IT
Max Diplo Adj* -- Each nation is able to adjust the diplomatic status
it has for any nation it has met.  The value of this option determines
the maximum adjustment from the current diplomatic setting which may
be made.  Keeping this value to 1 or 2 means that it will not be
possible to declare War on a nation without any prior warning.  Due to
the diplomacy change not being visible to other nations until after an
update, it is recommended that a low value be used for this option.
.IT
Bribery Value* -- This item sets a minimum relative value at which a
bribery is effective on an NPC nation.  It should be noted that the
new bribery system simulates real events that can take place when
nations send money to each other through envoys.  [ie., The envoy
you sent disappeared with your money and was last seen in the
vicinity of the French Riviera...]
.IT
Max Supply* -- Each army or other military unit may carry only so many
months worth of food and pay with them.  This selection determines the
limit as to how many months may be carried at one time.
.IT
Unit Sply Dist* -- This option controls the distance over which the
navies and caravans can provide support when they are on "Support"
status.  If this value is set to 0, they may only distribute to the
current sector, other settings indicate the maximum range over which
they may distribute, with a value of -1 being entered to have them
distribute based on the national communication range.
.IT
City Xfer Dist* -- This selection controls the maximum distance which
can separate two cities before they will be unable to transfer goods
directly using the "region-command" transfer mechanism.  Like the Unit
Sply Dist setting, entering a -1 will make it conform to the
communication range of the nation.
.UT
.CE
.CS "Changing Things Later On"
.PP
Once the world has already been created, there will probably be some
times when god will have to make adjustments to the state of the
campaign.  After all, what fun would it be to play god without being
able to "Play God"? :-)
.PP
Anyway, the most basic of changes, such as turning on or off campaign
options, is done through the an interface similar to that used during
world creation.  This is available through the "conqrun -E" command.
Those options which are not able to be changed will simply not allow
themselves to be adjusted.  This command provides the only method
available for god to change the campaign and add user passwords, if
they should ever need changing.
.PP
If god wishes to adjust the number of dice to be rolled during combat,
then the command "conqrun -Q" provides a interface for viewing and
adjusting the probably of various dice settings.  Any changes made to
the Combat Dice here will be reflected on the world options.
.PP
To get a quick listing of some campaign settings, simply use the
"conqrun -I" command.  Conquer will then display statistics about each
nation that might be of interest to a campaign deity.
.PP
Finally, there is the most powerful ability of god: the ability to
change anything in any portion of the campaign.  Such powers are
granted by just logging in as nation "god".  [I often refer to this as
"going into god-mode".]  Almost all of the commands available to
normal players will also function for god, and each will also provide
additional options which will allow god to manipulate almost any item.
It is simply a matter of determining which commands will do what for
god, and that is what the remainder of this section of the
documentation is for...
.CE
.CS "How to do God's Work"
.PP
Well, as I said earlier, many of the normal user commands will simply
function differently if you are logged in as god.  Also, when god
wishes to adjust some information about a given nation, god will, for
a short period of time, become that nation, and will then be able to
manipulate that nations' sectors, units or cities.  So, in order for
god to adjust items for a nation, that nation must not be logged in.
This is to prevent conflicts between adjustments god might make and
adjustments a player might make.
.PP
It should be noted that god will only assume the identity of a nation
for the duration of the individual commands.  For some commands, such
as "move-unit", god will automatically assume the identity of the
nation that owns the current unit.  For others, such as
"economy-report" or "enlist", god will be queried for the name of the
nation to become.
.PP
As Conquer is currently implemented, god will retain the login "lock"
on any nation that is adjusted.  This is to avoid any conflicts that
might develop from reading in the commands already issued by a nation
more than once.  So, if you wish to release a lock on an edited
nation, you must get out of god-mode.
.CE
.CS "Making Adjustments"
.PP
There are many different objects within Conquer, each of which has a
variety of commands which may affect it.  Whenever god accesses a
function which may make changes, a nation name to apply the changes to
will be asked for.  Some commands, such as "move-unit" will
automatically assume that the nation owning the current unit will be
the one affected.  Whenever god makes changes to a nation, that nation
may not be using Conquer, so once god is done makes changes, it would
be a good idea to log out.
.PP
The following list is to let god know which commands to use to get
various tasks done:
.IT
adjusting army units -- Army units may be adjusted using the
"army-report" or "group-report" functions.  The "command-unit"
function will also change a limited subset of unit information.
.IT
adjusting caravans -- Caravans may be manipulated using the
"caravan-report" or "command-unit" functions, much as an army unit.
.IT
adjusting naval units -- Navies may be manipulated using the
"navy-report" or "command-unit" functions, just like armies and
caravans.
.IT
relocating a unit -- Armies, navies and caravans may be relocated
using the "move-unit" function.  The currently selected unit will be
the one to be relocated.
.IT
creating a unit -- A unit may be created using the "enlist" command or
using the appropriate "{navy,caravan,army}-report" function.
.IT
disbanding a unit -- A unit may be deleted by using the "command-unit"
function or the appropriate "{navy,caravan,army}-report" function.
.IT
changing material amounts within a supply center -- The amount of raw
materials within a supply center may be adjusted using either the
"economy-report" interface or the "region-command" function.
.IT
changing material amounts stored on a unit -- The amount of raw
materials on a caravan or a navy may be adjusted using the
"navy-report" or "caravan-report" funtions, as appropriate.
.IT
changing supply levels on a unit -- The supply levels on units may be
adjusted individually using the "command-unit" function or the
appropriate "{navy,caravan,army}-report function.  Global adjustments
may be performed using the "region-command" function.
.IT
renaming a supply center -- Supply centers may be renamed by using the
"region-command" function.
.IT
renaming a nation -- Nations may be renamed using the "nation-info"
function.
.IT
changing the owner of a nation -- The login name for the owner of a
nation must be changed using the "nation-info" command. 
.IT
adjusting national statistics -- National attributes, such as the eat
and communication rates, may be adjusted using the "nation-info"
command.
.IT
deleting a nation -- Nations may only be deleted through the
"nation-info" command.
.IT
creating a nation -- Nations must be created using the conqrun
program, with the -a flag.  Additional NPC nations may be added by
creating a nation and then using the "nation-info" command to change
the nation to a non-player nation.
.IT
changing the constructions of a sector -- The constructions within a
sector may be changed using the "construct" command.  Removal of old
constructions is done by devastating the sector.
.IT
changing any other information about a sector -- The owner, contour,
designation, population, vegetation, trade good, and many other
traits of a sector may be changed by using the "change-designation"
function.
.IT
relocating people from a sector -- The "move-people" function may also
be used by god to change the population of two sectors at the same
time.
.IT
changing the magical powers a nation possesses -- The "magic-info"
function may be used to add to or take away from the magical powers a
nation has.
.IT
casting spells -- God may cast any spell upon any sector or unit by
using the "cast-spells" function.
.IT
call down a natural disaster -- The "cast-spells" function also allows
god to erupt volcanos, shake the earth, or call down other natural
disasters.
.IT
going to a specific capital -- It is possible to go to the capital of
any nation using the "jump-to-capital" command.
.IT
going to a specific city -- It is possible to go directly to any city
in the world using the "city-choose" function.
.IT
going to a specific unit -- To jump directly to any unit in any
nation, the "army-choose", "navy-choose", and "caravan-choose"
functions are available.
.IT
going to a specific sector -- To relocate to a given sector, just use
the "jump-to-pick" function.
.IT
giving a message to players during startup -- While in god-mode, use
the "edit-motd" function to start an editor for the message of the
day.  The "conquer -M" program may also be used, if you do not wish to
log into Conquer just to edit the MOTD.
.IT
kick players out of the game -- You may prevent players from logging
into Conquer by using the "conqrun -T" program.  This should give them
a short delay before they are automatically logged out, if they are
currently in the program.
.IT
update the game -- A very important aspect of godhood is having to
update the world.  The program "conqrun -x" is used to update the
program, and depending on how it was compiled will wait until all
players have logged out, preventing further logins, and then update
the world.  Using this program with an automated script program, such
as "at" on Unix, can make life much easier for a god.
.UT
.PP
A detailed listing of which keys are bound to what functions is listed
elsewhere in the documentation.
.RF "Functions" "The List of Commands"
.CE
