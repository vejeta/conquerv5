.CH "Conquer Customization Settings"
.CS "Tailoring Conquer"
.PP
During the writing of Conquer, there were many times when people have
said that they wanted things done differently from the way I
envisioned it.  While I could very easily have told them where they
can take their "valuable suggestions" :-), I found that there were one
or two of them that were bigger than me.  So, I decided that I would
listen politely to the suggestions and then, if possible, make it an
option.  Of course, I still decided what options were the "default"
ones, so I ended up with the last say anyway.
.PP
There are now many things within Conquer which allow the user to
specify how the user interface is to behave for them.  The display
modes can be created or adjusted at will, the key bindings can be
manipulated in almost any manner, and there are a number of flags
which can be set based on the tastes of the user.
.PP
Except for the display modifications, which are under the
"custom-display" function
.RF "Display" "Conquer Data Display Routines"
, all of the configuration commands are kept under the
"conquer-options" command.  This command provides a list of options by
which the user may set a number of different preferences, including
the rebinding of the entire key map of Conquer.
.PP
It should be noted that in all of the portions of Conquer containing
key maps, the key bindings may be adjusted by calling the
"conquer-options" command while within that mode.
.CE
.CS "The conquer-options Command"
.PP
Conquer currently supports twenty-one different options under the
"conquer-options" function.  After executing the "conquer-options"
function, type in the name of the desired option to make use of it.
The complete list of options, and their meanings, is:
.IT
all-blanks -- On detailed zoom, leave any unused bottom characters of
the sector blank, as opposed to the normal method of using underlines.
.IT
bind-key -- Interactively assign a set of keystrokes to call a
function.
.IT
bottom-lines -- For detailed zooms, set the bottom of characters of
water sectors to be underlines, unless either "water-bottoms" or
"all-blanks" is enabled.
.IT
check-keys -- Verify that all key bindings in the current mode are
properly set.  Any conflicts or missing functions will be reported as
they are encountered.  This is done each time Conquer is started.
.IT
expert -- This toggles the setting of the "expert" option.  If Conquer
considers you an expert, it will no longer verify most commands, and
will assume you know what you are doing.  Any complaints resulting
from the use of this option will be given due consideration.
.IT
gaudy -- When enabled, any time the nation's name is encountered
within the news, it will be highlighted so as to improve its
readability.
.IT
header-mode -- If on, the header summary will be shown upon entering
the mail editor, otherwise the next unread mail message will
immediately be displayed.
.IT
info-mode -- It is possible to toggle whether or not more information
about the current sector will be visible during the Conquer map
display.  The command "toggle-infomode" is also provided, since this
is an option which should be enabled or disabled with ease.
.IT
mail-check -- If on, and your system supports it, Conquer will check
the mail spool to determine if any real electronic has been sent.
Turning this option off will prevent "hanging" if an NFS mounted mail
spool directory becomes inaccessible.
.IT
pager-offset -- Specify how many lines down the screen the Conquer
pager focus line is.
.IT
pager-scroll -- Specify how many lines the Conquer pager will scroll
when paged.
.IT
pager-tab -- The number of spaces used to represent a tab character
are controlled by this option.  A value of zero indicates that the tab
should remain unconverted.
.IT
read-opts -- Read in a file that is in the ".conqrc" format.
.IT
rebind-key -- Assign a currently used key binding to another function.
.IT
recenter-map -- Assign the current sector location to be the relative
center of the national coordinate map.  This provides a method of
aligning coordinate systems between nations.
.IT
reset-keys -- Restore the key bindings to their default settings,
making it possible to at least recover from any keybinding problems.
.IT
store-opts -- Store all of the current Conquer configuration options
into an automatically generated customization file.   A query is made
to determine the exact file location for such an operation.  Note that
"nation" and "campaign" are not stored to avoid the possibility of
overwriting prior defaults.
.IT
supply-level -- When a military unit is created, it will automatically
be built with food and payment for a given number of turns.  This
option controls how many such supplies are provided.
.IT
terminal-bell -- If on, Conquer will beep, if possible, whenever an
error occurs.
.IT
unbind-key -- Remove a given key binding from the current key map.
.IT
water-bottoms -- If on, the bottom portion of water sectors will
always be shown with water, and not underlines, in the detailed zoom.
.UT
.CE
.CS "Building a Customization File"
.PP
Conquer provides support for customization files, which take the form
of plain text files containing configuration commands.  These files
can be created manually, or can be automatically generated using the
"store-opts" option from the "cnoquer-options" function.
.PP
The configuration file consists of a list of single line commands,
along with comments, which are indicated by a number sign (#) as the
first non-space character.  The list of supported customization
commands, and their meanings, is:
.IT
include: path-to-file
.TT
Through the use of the "include" customization command, the user is
able to specify additional customization files to be read in by
Conquer.  This is very useful for breaking apart the customization
environment into separate parts for ease of control.  [In the future,
I plan to enable conditional include options to allow customization
based on the terminal type or other information]
.IT
nation: nationname
.NT
name: nationname
.TT
Both of the above commands are used to specify a default nation to be
used by the player.  These are useful to avoid having to always
specify the nation name on the command line.  This may be overridden
by either the command line or the environment variable.
.IT
campaign: campaignname
.NT
data: campaignname
.NT
directory: campaignname
.TT
These customization commands provide the user with a method of
specifying a default campaign to be accessed.  They may be overridden
by either the command line or the environment variable.
.IT
all-blanks
.NT
bottom-lines
.NT
expert
.NT
gaudy
.NT
header-mode
.NT
info-mode
.NT
mail-check
.NT
terminal-bell
.NT
water-bottoms
.TT
All of the above commands turn the appropriate option on.  If it is
proceeded by a `!' character, that option is turned off.
.IT
supply-level: #
.TT
Set number of supply units each unit should receive when enlisted.
.IT
pager-tab: #
.NT
pager-scroll: #
.NT
pager-offset: #
.TT
The above options, followed by a numeric indicate that the appropriate
variables should be given the indicated value.
.IT
zoom-level: detail
.TT
The zoom level setting indicates what the beginning zoom setting will
be.  The zoom level may be either "detail", "medium", or "full".
.IT
contour Type=C
.NT
vegetation Type=C
.NT
designation Type=C
.TT
Specify what characters are to be used to display the indicated
contour, vegetation or designation types.
.IT
unbind-key map "keystr"
.NT
bind-key map "keystr" function-name
.NT
rebind-key map "keystr" function-name
.TT
The above commands allow for the specification of a key binding for an
indicated Conquer key map.  The currently available maps are global,
mail, magic, move, and reader.  The key string is a set of characters
enclosed by either single or double quotes.  The key string should be
composed entirely of displayable characters, with a `^' character
being interpreted as an indicator for control characters, and the '\e'
character being used to give "C" style character sequences or just to
quote a given character.
.IT
display-mode "Name" highlight style
.NT
display-mode "Name" focus hexloc
.NT
display-mode "Name" hexloc style
.TT
Create or modify display modes and its display, focus or highlighting
settings.  A hexloc indicates the position which can be either
low-left, low-right, up-left or low-right.  The styles are the display
or highlight settings which are available to the user.  The focus
command is used to indicate which of the four hexloc positions is to
be considered the most important.
.IT
default-display highlight style
.NT
default-display focus hexloc
.NT
default-display hexloc style
.TT
Set the default display mode settings in much the same manner as the
named display modes.  This default display method is used Conquer is
first entered.
.UT
.CE
.CS "The Environment Variable"
.PP
In addition to the the customization file, Conquer provides support
for an the "CONQ_OPTS" environment variable.  I won't get into a
discussion on how to set the environment variable here, since that
varies with each user's shell or operating system, and is usually easy
to do.  But, I will describe just what things can be set using the
environment variable.
.PP
The environment variable is not quite as robust as the customization
file, but it does provide a quick way of overriding many options which
were set in such a file, without having to permanently change that
file.  Anyway, the environment variable supports a list of mostly one
character items which correspond with Conquer options:
.IT
B
.TT
A `B' character indicates to enable the "all-blanks" option.  It may
be disabled by preceding it with a `!' character.
.IT
b
.TT
A `b' character indicates to enable the "bottom-lines" option.  It too
may be disabled with a preceding `!' character.
.IT
e
.TT
An `e' represents the "mail-check" option.  It may be disabled by
preceding it with a `!'.
A 
.IT
G
.TT
The "gaudy" highlighting option is enabled with a single `G' option,
and may be disabled by preceding it with a `!' character.
.IT
H
.TT
The "header-mode" is controlled with a `H' character.  If preceded by
a `!' character, it may be disabled.
.IT
i
.TT
Choosing to start with info-mode enabled is done using the `i'
character within the environment variable.  The `!' will disable it.
.IT
t
.TT
Enabling the "terminal-bell" option is done with a `t'.  A preceding
exclamation point will disable it.
.IT
w
.TT
The "water-bottoms" option is set using the `w' flag, or disabled with
a '!w' sequence.
.IT
x
.TT
Becoming an "expert" is no problem, just use the `x' character within
the environment variable.  Disable with an exclamation point.
.IT
name=nationname
.TT
The name of the nation to be used may be specified using this option.
Be sure to use a comma to end it, if there are any additional options
after it.
.IT
data=campaign
.TT
Finally, the campaign may be specified through the use of this option.
It should also be distinguished from following option through the use
of a comma.
.UT
.PP
As an example, if a player using csh wished to enable "gaudy" and
"water-bottoms", along with disabling "all-blanks", they could use the
following, which would also set their name and campaign:
.SB
        setenv CONQ_OPTS 'Gw!b,name=Mordor,data=MiddleEarth'
.SE
.PP
While the environment variable cannot be used to specify display
modes, screen characters, or pager variables, it does have a number of
useful features which can make it easy to override customization file
settings.
.CE
.CS "The Command Line"
.PP
There are two main programs which compose the Conquer system: conquer,
the user interface, and conqrun, the administrative interface.  If you
wish to get an exact list of the options available when running each
program, you may use "conquer -\?" or "conqrun -\?" respectively.
.PP
To the sake of completeness, I will list all of the command line
options for both here as well.  For the conquer command, the options
are:
.SB
  Command line format: conquer [-nc] [-BbeGHhilMpsw -d DIR -nNAT]
        -nc      do not read in .conqrc file [first option only]
        -n NAT   play as nation NAT
        -d DIR   use to access different campaign
        -B       use blanks, not underlines [toggle]
        -b       always underline water [toggle]
        -e       don't check for system email [toggle]
        -G       highlight name in news [toggle]
        -H       use header-mode option [toggle]
        -i       don't set info-mode option [toggle]
        -t       don't enable warning bells [toggle]
        -w       draw water with no underlines [toggle]
        -X       enable expert status [toggle]
        -l       dislay list of user logged in
        -h       print help text
        -P       print a map with highlighting
        -p       print a map
        -s       print scores
        -M       edit the MOTD, if allowed
        -D       dump nation information
.SE
.PP
Those selections with the [toggle] at the end are the Conquer
configuration options, which indicate, by the presence or absence of
the string "don't ", what will be done if they are enabled on the
command line.  The conqrun command also have a number of selections:
.SB
  Command line format: conqrun [-nc] [-ACEQITZamx -dDIR]
        -nc      do not read in .conqrc file [first option only]
        -E       edit the world campaign settings
        -I       display some statistics
        -A       add some npc nations to the nations file
        -Q       examine the combat dice probabilities
        -T       (dis/en)able logins; 120 sec time to finish for those in
        -Z       delete all npc nation specifications
        -a       add new player
        -d DIR   to specify a different game directory
        -o ofile send update output to the given file
        -m       make a world
        -x       execute program
.SE
.PP
As you can tell from the above lists, you may avoid reading in the
.conqrc file altogether by having the "-nc" flag as the first entry.
Most of the Conquer settings are just parallels of the environment
variable options, except that they act as toggles.  To find out
exactly what each option does, you can probably just try it out and
see what happens.
.CE
