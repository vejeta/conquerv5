.CH "Communication Between Players"
.CS "Mail Messages"
.PP
Discussions and negotiations play a crucial role in the development of
national diplomacy.  Often, it is communication between players that
forms or dismantles treaties and alliances.  Double dealing and
straight forward honesty must be carefully balanced when dealing with
other players, or they will be able to determine exactly what you are
up to.
.PP
To enable communication between players, there is the electronic mail
system.  Using this system, nations will receive mail messages
reporting concerning economic statuses, combat reports, and
communications from other countries.  There are two commands providing
direct access to the mail system.  The "read-mail" function is used to
read and respond to any mail which you have received, and the
"mail-nation" function is available to send mail to other nations.
.PP
To help your nation determine when any new mail has arrived, a small
message saying "You have Conquer Mail" will be displayed if Conquer
detects that there is any mail which has not yet been read.  A similar
message will be displayed if the newspaper has not yet been read or,
on some machines, if there is some real electronic mail waiting to be
read.  It is possible to control the behavior of the last operation
using the "mail-check" setting in the "conquer-options" function.
.RF "Functions" "The List of Commands"
.CE
.CS "Reading Mail"
.PP
The mail reader provided with Conquer is similar to that found in
normal Unix mail systems.  There is a summary listing, as well as a
full screen display of the current mail message.  It is possible to
select which of these two modes are used upon startup by setting the
"header-mode" option within the "conquer-options" function.
.RF "Functions" "The List of Commands"
.PP
The mail reader has its own set of keybindings and functions:
.IT
conquer-options (`O') --  Adjust the Conquer environment by changing
various options.  Use this function to adjust keybindings for the
mode.
.RF "Functions" "The List of Commands"
.IT
reader-backward (`b') -- Scroll the current message backward one page.
.IT
reader-bottom (`>') -- Shift the view to the bottom of the current
message.  This can be especially useful for viewing the summary at the
bottom of the economic reports.
.IT
reader-delete (`D') -- Mark the current mail message for deletion.
The deletion will not actually take place until the "reader-purge" or
the "reader-exit" functions have been used.
.IT
reader-delete-all (`C') -- Mark all messages for deletion.  Actual
deletion will take place when either the "reader-purge" or the
"reader-exit" function has been used.
.IT
reader-delete-and-next (`d') -- Mark the current message for deletion
and go to the next undeleted message.  The message will then be
removed whenever the "reader-exit" or "reader-purge" functions are
used.
.IT
reader-delete-and-previous (`^D') -- Mark the current message for
deletion and find the closest previous undeleted message.  Permanent
deletion will only take place when a purge is done.
.IT
reader-delete-read-mail (`c') -- Mark all previously read mail for
deletion.  The messages will only be removed after the "reader-purge"
or the "reader-exit" functions have been used.
.IT
reader-down-one (`^J',`^M') -- Move the current message forward one
line.
.IT
reader-exit (`q') -- Purge any mail marked for deletion, and exit the
mail reader.
.IT
reader-forward (`b') -- Scroll the current mail message forward one
page.
.IT
reader-goto (`G',`g') -- Go to a specified mail message.
.IT
reader-help (`?') -- Display the list of mail reader functions and
keybindings.
.IT
reader-mail (`M',`m') -- Send a mail message to another nation.  This
is equivalent to using the "mail-nation" function described later.
.IT
reader-mail-forward (`f') -- Send a copy of, or "forward", the current
message to another nation.  This will enter the mail-editor with the
mail message already written as part of the message.
.IT
reader-mail-reply (`r') -- Send a reply to the nation who sent the
current mail message.  The subject line and address will already be
properly formatted.
.IT
reader-next (`^N',DOWN-ARROW,`J',`N') -- Move to the next mail
message, regardless of the status of the next message.
.IT
reader-next-undeleted (`j',`n') -- Move to the next undeleted mail
message.
.IT
reader-previous (`^P',UP-ARROW,`K',`P') -- Move to the previous mail
message, regardless of the status of the previous message.
.IT
reader-previous-undeleted (`k',`p') -- Move to the previous undeleted
mail message.
.IT
reader-purge (`x') -- Permanently remove any mail messages which are
marked for deletion.
.IT
reader-quit (`Q') -- Exit the mail reader, leaving any mail marked for
deletion intact.
.IT
reader-quoted-forward (`F') -- Forward the current message, quoted, to
another nation.  Quoting means that the mail message will have each
line preceded by a sequence of characters indicating that it has been
forwarded.  This function is very useful for composing a message based
on mail which was received from another nation.
.IT
reader-quoted-reply (`R') -- Send a reply to the sender of the current
mail message and include the original message text within the mail
message.
.IT
reader-toggle (`H',`h') -- Toggle between header, or summary mode, and
the full screen message mode.
.IT
reader-top (`<') -- Shift the view of the current mail message to the
top of the current mail message.
.IT
reader-undelete (`u') -- Remove a deletion marker from the current
message, or the nearest previous mail message marked for deletion.
.IT
reader-unread (`U') -- Remove the indicator which shows that a mail
message has already been read.  If the current mail message has no
such marker, the nearest previous mail message with one will have it
removed.
.IT
reader-up-one (`^H',DEL) -- Move the current message backward one
line.
.IT
redraw-screen (`^L',`^R') -- Clear the screen and redisplay the
information.
.IT
ignore-key (`^B',`^F',LEFT-ARROW,RIGHT-ARROW) -- Ignore these keys
when used.
.UT
.CE
.CS "Sending Mail"
.PP
Getting into the mail editor is done through using either the
"mail-nation" function, or through the reply, mail or forward options
within the mail reader.  Once the mail editor has been entered, there
are a number of commands and options available to help compose a mail
message.
.PP
The editor screen will look something like:
.SB
To:
Subject:



 Conquer Version 5.0: Mail Editor                     Hit "ESC-?" for Bindings
-------------------------------------------------------------------------------
Enter a nation, "god" (to mail deity), or "news" (to send a personal)
.SE
.PP
The first line will contain the list of nations to which the mail
message is being sent.  Typing in a nation name on that line will add
it to the list of nations who will receive the message.  It is not
possible for two messages to be sent to the same nation at the same
time, so you might receive an error message indicating that some other
nation is already sending mail to that nation.  If you should, waiting
a few minutes for the other player to finish will take care of the
problem.
.PP
The second line is called the subject line, and is used to indicate a
summary or topic for the mail message.  A subject may only be one line
long, so a short phrase is best.  If no subject is provided, the
string "[none]" will be inserted when the message is sent.
.PP
The area below the subject line should be used to enter in the text of
the message.  The text can consist of anything, but sending useless
mail to other nations might not be looked on favorably.  For those
familiar with "emacs" keybindings, the Conquer mail editor shouldn't
pose much of a problem.  In the future, support for a "vi" mode will
also be available.  Regardless of how you feel about the mail editor,
it is possible to use your own editor of choice to compose the
message, using the "mail-spawn-editor" function.
.PP
Once the mail message is complete, you may deliver it using the
"mail-exit" or "mail-send" functions.  If you should decide to abort
the mail editor and discard everything, simply use the "mail-quit"
function.  The next section describes the full list of mail editor
commands, along with their default key bindings.
.CE
.CS "List of mail mode functions"
.PP
The full list of functions and default keybindings available within
the mail editor is:
.IT
conquer-options (`^O') -- Adjust the Conquer environment by changing
various options, including keybindings for the mail editor.
.IT
ignore-key (`^C') -- Just pretend this key press didn't really happen.
.IT
mail-backward (`^B',LEFT-ARROW) -- Move the cursor one character to
the left.
.IT
mail-beginning-of-line (`^A') -- Move the cursor to the beginning of
the current line.
.IT
mail-delete-backward (`^H',DEL) -- Remove the character directly to
the left of the cursor.
.IT
mail-delete-forward  (`^D') -- Delete the character under the cursor.
.IT
mail-downline (`^N',DOWN-ARROW) -- Move cursor down one line.
.IT
mail-end-of-line (`^E') -- Move the cursor to the end of the current
line.
.IT
mail-exit (`^X') -- Deliver the mail message and then exit the mail
editor.
.IT
mail-forward (`^F',RIGHT-ARROW) -- Move the cursor one character to
the right.
.IT
mail-help ("ESC-?") -- List the current functions and keybindings for
the mail editor.
.IT
mail-kill-to-beginning (`^U') -- Delete all characters from the left
of the cursor to the beginning of the current line.
.IT
mail-kill-line (`^K') -- Delete all characters from the cursor until
the end of the current line.
.IT
mail-newline (`^J',`^M') -- Break the current line at the cursor and
create a new line of text.
.IT
mail-quit (`^G') -- Leave the mail editor after discarding the mail
message.
.IT
mail-send (`^W') -- Send the mail message to all of the recipients,
and remain in the mail editor.
.IT
mail-spawn-editor ("ESC-$") -- Use whatever editor, as determined by
the EDITOR environment variable, to edit the current message.
.IT
mail-toggle (`^T') -- Toggle between overwrite and insert mode in the
editor.
.IT
mail-upline (`^P',UP-ARROW) -- Move the cursor up one line.
.IT
redraw-screen (`^L',`^R') -- Redraw the screen display.
.UT
.CE
.CS "News Postings"
.PP
The newspaper is available to all users and contains a summary of
events taking place during recent months.  A limited number of
newspapers will be kept in storage, so reading them frequently is
recommended.  If the latest newspaper has not been read or if
something has been added to it, a message stating that "There is
Conquer News" will be displayed at the bottom of the main map screen.
Using the "read-paper" command will display the list of currently
available newspapers, by date, with the most recent editions being
listed first.
.PP
Newspaper are divided into five sections: headlines, economics, real
estate, warfare and personals.  The headlines section will contain any
important information such as declarations of war and peace, leader
and monster recruitment and notifications when two nations meet.  The
economics report lists any famines or desertions which have taken
place in the world, giving an idea of how well nations are being
managed.  The real estate listings show who took what land, and from
whom.  The warfare section gives a run down of who was involved in the
battles around the world.  Finally, the personals section is where any
mail messages sent to "news" will appear, so that they may be read by
all of the players.
.CE
.CS "Economic Reports"
.PP
During each update, mail will be sent from the updating program
describing events taking place within the nation.  Some of the mail
messages will describe the economic conditions within the nation and
others will contain reports about battles.
.RF "Warfare" "Military Units and Combat"
.PP
The economic reports come in two different forms: production reports
and consumption reports.  The production reports will provide a
detailed summary of how much of what materials each region produced.
If you see that some towns and cities are not generating enough of
their own raw materials, you might need to add more farms, mines, or
lumberyards within the region to improve the situation.  The
consumption report gives a summary of how much was consumed on a
nationwide basis.
.RF "Economics" "Economic Management"
.CE
