.\" || 
.\" || This package is designed as a filter to allow the same
.\" || roff data file to be used to provide proper pagination
.\" || for both the nroff and troff (psroff) output.
.\" ||
.\"
.\" -- Determine what all of the fonts to be used are
.\"
.if t .fp 1 R \{
.\"
.\" -- Define the Fonts
		\" font for the main body and page numbering
.fp 2 B		\" font for titles and such
.fp 3 C 	\" font for command examples and screen displays
.cs 3 21.5
.fp 4 S	 	\" just for completion
.po 1.0i
.wh -1i PB \}
.\"
.\" -- Define the strings for the program title and default section heading
.ds Ct "Conquer Version 5.0 Beta
.ds Ch "Help File
.nr Hs 0
.\"
.\" -- Define the macro for the conquer sectional heading
.de CH
.ds Ch \\$1
.nr Hs 0
.if t .ps 18 \\{
.vs 23
.ft 2
.bp
.if e .bp
.sp 1.0i
.ce 1
\\$1
.sp 0.5i \\}
..
.\"
.\" -- Define the macro to display the page numbering (in troff)
.de PB
'bp
'sp 0.75i
.ev 2
.ie \\n(Hs .ft 1 \\{
.ps 8
.vs 10
.ie e .tl '%'\\*(Ct''	\" show the documentation title on even pages
.el .tl ''\\*(Ch'%' \\}	\" show the section heading on odd pages
.el .sp 0.2i
.ev
'sp 0.3i
..
.\"
.\" -- Define a macro for the page breaks
.de NP
.if n ^L
.if t .if \\n(dn>=\\n(.t .bp	\" need enough space to fit on the page
..
.\"
.\" -- Define a macro to handle documentation referencing
.de RF
[For more information, see
.ie t \\S'-15'\\$2\\S'0']
.el \\$1]
..
.\"
.\" -- Beginning of conquer sectioning
.de CS
.ev 1
.nr Hs 1
'di CZ	\" store text into the CZ "macro"
'nf
.if t .ft 2 \\{
.vs 12
.sp 1 \\}
\\$1
.if t .ft 1 \\{
.vs 14 \\}
'fi
..
.\"
.\" -- Ending of conquer sectioning
.de CE
.sp
.di
.in 0.0i
.if n .ll 6.5i
.NP
.nf
.CZ
.ev
..
.\"
.\" -- Paragraph separations
.de PP
.sp 1
.ft 1
.fi
.if n .in 0.5i \\{
.ll 7.0i \\}
.if t .ti +0.25i
..
.\"
.\" -- Screen Examples (tightened vertical spacing in courier)
.de SB
.sp 1
.nf
.if t .ft 3 \\{
.vs 9 \\}
.if n .in -0.5i
..
.de SE
.fi
.if t .ft 1 \\{
.vs 14 \\}
.if n .in +0.5i
..
.\"
.\" -- Tables (constant width courier)
.de TB
.sp 1
.nf
.if t .ft 3 \\{
.vs 11 \\}
.if n .in -0.5i
..
.de TE
.fi
.if t .ft 1 \\{
.vs 14 \\}
.if n .in +0.5i
..
.\"
.\" -- Indented Text Macros
.de IT
.fi
.ft 1
.sp 1
.in 1.0i
.ti 0.5i
.if t .vs 12
..
.de NT
.br
.fi
.ft 1
.in 1.0i
.ti 0.5i
.if t .vs 12
..
.de ST
.fi
.ft 1
.sp 1
.in 1.0i
.if t .vs 12
..
.de TT
.fi
.ft 1
.in 1.0i
.if t .vs 12
..
.de UT
.ft
.in 0.0i
.if t .vs 14
..
.\"
.\" -- Now set the general environment
'nf
'ft 1
