# define CGA 1
# define ESC 27
# define YES 1
# define NO 0
# define NO_SHADOW 0
# define HALF_SHADOW -1

/* various menu definitions */
/* character following `^' is a hot key */
char *mainmenu[] =	{
						"^Cursor Movement",
						"^File",
						"^Search",
						"^Delete",
						"^Exit"
					} ;

char *cursormenu[] =	{
						"^Start of File",
						"End of ^File",
						"^Top of screen",
						"^Bottom of screen",
						"Start of ^Line",
						"^End of Line",
						"^Word Left",
						"Word ^Right",
						"Page ^Up",
						"Page ^Down",
						"Retur^N"
					} ;

char *filemenu[] = {
					"^Load",
					"^Pick",
					"^New",
					"^Save",
					"Save ^As",
					"^Merge",
					"^Change dir",
					"^Output to printer",
					"Re^Turn"
				  } ;

char *searchmenu[] = {
						"^Find",
						"Find & ^Replace",
						"Repeat ^Last find",
						"^Abort operation",
						"^Go to line no",
						"Re^Turn"
					 } ;

char *deletemenu[] =	{
						"^Delete line",
						"To ^End of line",
						"To ^Beginning of Line",
						"Word ^Right",
						"Re^Turn"
					} ;

char *exitmenu[] = {
					"^Exit",
					"^Shell",
					"Re^Turn"
				  } ;

/* most recent files edited */
char *pickfile[5] = {
					"              ",
					"              ",
					"              ",
					"              ",
					"              "
				} ;

/* buffer in which files are loaded and manipulated */
char *buf ;

unsigned maxsize ;
char *startloc, *curscr, *currow, *endloc ;
char searchstr[31], replacestr[31], filespec[30], filename[17] ;

void interrupt (*old1b)() ;
void interrupt handler() ;
void interrupt (*old23)() ;
char *search ( char * ) ;

int ascii, scan, pickfileno, no_tab ;
int curr = 2, curc = 1, logc = 1, logr = 1 ;
int skip, findflag, frflag, saved = YES, ctrl_c_flag = 0 ;

char far *vid_mem ;
char far *ins = ( char far * ) 0x417 ;

/* writes a character in specified attribute */
writechar ( int r, int c, char ch, int attb )
{
	char far *v ;

	v = vid_mem + r * 160 + c * 2 ;  /* calculate address corresponding to row r and column c */
	*v = ch ;  /* store ascii value of character */
	v++ ;
	*v = attb ;  /* store attribute of character */
}

/* writes a string in specified attribute */
writestring ( char *s, int r, int c, int attb )
{
	while ( *s != '\0' )
	{
		/* if next character is the hot key of menu item */
		if ( *s == '^' )
		{
			s++ ;

			/* if hot key of highlighted bar */
			if ( attb == 15 )
				writechar ( r, c, *s, 15 ) ;
			else
				writechar ( r, c, *s, 113 ) ;
		}
		else
		{
			/* if next character is hot key of "Yes", "No", "Cancel", etc. */
			if ( *s == '$' )
			{
				s++ ;
				writechar ( r, c, *s, 47 ) ;
			}
			else
				writechar ( r, c, *s, attb ) ;  /* normal character */
		}

		c++ ;
		s++ ;
	}
}

/* saves screen contents into allocated memory */
savevideo ( int sr, int sc, int er, int ec, char *buffer )
{
	char far *v ;
	int i, j ;

	for ( i = sr ; i <= er ; i++ )
	{
		for ( j = sc ; j <= ec ; j++ )
		{
			v = vid_mem + i * 160 + j * 2 ;
			*buffer = *v ;  /* store character */
			v++ ;
			buffer++ ;
			*buffer = *v ;  /* store attribute */
			buffer++ ;
		}
	}
}

/* restores screen contents from allocated memory */
restorevideo ( int sr, int sc, int er, int ec, char *buffer )
{
	char far *v ;
	int i, j ;

	for ( i = sr ; i <= er ; i++ )
	{
		for ( j = sc ; j <= ec ; j++ )
		{
			v = vid_mem + i * 160 + j * 2 ;
			*v = *buffer ;  /* restore character */
			v++ ;
			buffer++ ;
			*v = *buffer ;  /* restore attribute */
			buffer++ ;
		}
	}
}

/* displays filled box with or without shadow */
menubox ( int sr, int sc, int er, int ec, char fil, char shad )
{
	int i, j ;

	for ( i = sr ; i < er ; i++ )
		for ( j = sc ; j < ( ec - 1 ) ; j++ )
			writechar ( i, j, ' ', fil ) ;

	/* if no shadow is required for the filled box */
	if ( shad == NO_SHADOW )
	{
		for ( i = sr; i <= er ; i++ )
		{
			writechar ( i, ec, ' ', fil ) ;
			writechar ( i, ( ec - 1 ), ' ', fil ) ;
		}

		for ( j = sc ; j <= ec ; j++ )
			writechar ( er, j, ' ', fil ) ;
	}
	else
	{
		/* if half shadow required for the filled box */
		if ( shad == HALF_SHADOW )
		{
			for ( i = sr ; i <= er ; i++ )
				writechar ( i, ( ec - 1 ), 220, 112 ) ;

			for ( j = sc + 1 ; j < ec ; j++ )
				writechar ( er, j, 223, 112 ) ;
		}
		else
		{
			/* create normal shadow */
			for ( i = sr + 1 ; i <= er ; i++ )
			{
				writechar ( i, ec, ' ', shad ) ;
				writechar ( i, ( ec - 1 ), ' ', shad ) ;
			}

			for ( j = sc + 2 ; j <= ec ; j++ )
			writechar ( er, j, ' ', shad ) ;
		}
	}
}

/* draws a double-lined box */
drawbox ( int sr, int sc, int er, int ec, int attr )
{
	int i ;

	/* draw horizontal lines */
	for ( i = sc + 1 ; i < ec ; i++ )
	{
		writechar ( sr, i , 205, attr ) ;
		writechar ( er, i, 205, attr ) ;
	}

	/* draw vertical lines */
	for ( i = sr + 1 ; i < er ; i++ )
	{
		writechar ( i, sc, 186, attr ) ;
		writechar ( i, ec, 186, attr ) ;
	}

	/* display corner characters */
	writechar ( sr, sc, 201, attr ) ;
	writechar ( sr, ec, 187, attr ) ;
	writechar ( er, sc, 200, attr ) ;
	writechar ( er, ec, 188, attr ) ;
}

/* displays or hides cursor */
size ( int ssl, int esl )
{
	union REGS i, o ;

	i.h.ah = 1 ;  /* service number */
	i.h.ch = ssl ;  /* starting scan line */
	i.h.cl = esl ;  /* ending scan line */
	i.h.bh = 0 ;  /* video page number */

	/* issue interrupt for changing the size of the cursor */
	int86 ( 16, &i, &o ) ;
}

/* gets ascii and scan code of key pressed */
getkey()
{
	union REGS i, o ;

	/* wait till a key is hit */
	while ( !kbhit() )
	{
		/* if Ctrl - C has been hit */
		if ( ctrl_c_flag )
		{
			/* erase the characters ^C */
			displayline ( currow, logr + 1 ) ;
			gotoxy ( logc + 1, logr + 2 ) ;
			ctrl_c_flag = 0 ;
		}
	}

	i.h.ah = 0 ;  /* service number */

	/* issue interrupt */
	int86 ( 22, &i, &o ) ;

	ascii = o.h.al ;
	scan = o.h.ah ;
}

/* pops up a menu vertically */
popupmenuv ( char **menu, int count, int sr, int sc, char *hotkeys, int helpnumber )
{
	int er, i, ec, l, len = 0, area, choice ;
	char *p ;

	size ( 32, 0 ) ;  /* hide cursor */

	/* calculate ending row for menu */
	er = sr + count + 2 ;

	/* find longest menu item */
	for ( i = 0 ; i < count ; i++ )
	{
		l = strlen ( menu[i] ) ;
		if ( l > len )
			len = l ;
	}

	/* calculate ending column for menu */
	ec = sc + len + 5 ;

	/* calculate area required to save screen contents where menu is to be
	   popped up */
	area = ( er - sr + 1 ) * ( ec - sc + 1 ) * 2 ;

	p = malloc ( area ) ;  /* allocate memory */

	/* if allocation fails */
	if ( p == NULL )
		error_exit() ;

	/* save screen contents into allocated memory */
	savevideo ( sr, sc, er, ec, p ) ;

	/* draw filled box with shadow */
	menubox ( sr, sc + 1, er, ec, 112, 15 ) ;

	/* draw a double lined box */
	drawbox ( sr, sc + 1, er - 1, ec - 2, 112 ) ;

	/* display the menu in the filled box */
	displaymenuv ( menu, count, sr + 1, sc + 3 ) ;

	/* receive user's choice */
	choice = getresponsev ( menu, hotkeys, sr, sc + 2, count, helpnumber ) ;

	/* restore original screen contents */
	restorevideo ( sr, sc, er, ec, p ) ;

	/* free allocated memory */
	free ( p ) ;

	size ( 5, 7 ) ;  /* set cursor to normal size */
	return ( choice ) ;
}

/* displays menu vertically */
displaymenuv ( char **menu, int count, int sr, int sc )
{
	int i ;

	for ( i = 0 ; i < count ; i++ )
	{
		writestring ( menu[i], sr, sc, 112 ) ;
		sr++ ;
	}
}

/* receives user's choice for the vertical menu displayed */
getresponsev ( char **menu, char *hotkeys, int sr, int sc, int count, int helpnumber )
{
	int choice = 1, len, hotkeychoice ;

	/* calculate number of hot keys for the menu */
	len = strlen ( hotkeys ) ;

	/* highlight the first menu item */
	writestring ( menu[choice - 1], sr + choice, sc + 1, 15 ) ;

	while ( 1 )
	{
		/* receive key */
		getkey() ;

		/* if special key is hit */
		if ( ascii == 0 )
		{
			switch ( scan )
			{
				case 80 :  /* down arrow key */

					/* make highlighted item normal */
					writestring ( menu[choice - 1], sr + choice, sc + 1, 112 ) ;

					choice++ ;
					break ;

				case 72 :  /* up arrow key */

					/* make highlighted item normal */
					writestring ( menu[choice - 1], sr + choice, sc + 1, 112 ) ;

					choice-- ;
					break ;

				case 77 :  /* right arrow key */
					return ( 77 ) ;

				case 75 :  /* left arrow key */
					return ( 75 ) ;

				case 59 :  /* function key F1 for help */

					/* if current menu is file menu */
					if ( helpnumber == 3 )
					{
						/* if highlighted bar is not on return */
						if ( choice != 9 )
						{
							/* call with appropriate help screen number */
							displayhelp ( 8 + choice - 1 ) ;
						}
						break ;
					}

					/* if current menu is exit menu */
					if ( helpnumber == 6 )
					{
						/* if highlighted bar is not on Return */
						if ( choice != 3 )
						{
							/* call with appropriate help screen number */
							displayhelp ( 16 + choice - 1 ) ;
						}
						break ;
					}

					/* if current menu is other than file menu or exit menu */
					displayhelp ( helpnumber ) ;
			}

			/* if highlighted bar is on first item and up arrow key is hit */
			if ( choice == 0 )
				choice = count ;

			/* if highlighted bar is on last item and down arrow key is hit */
			if ( choice > count )
				choice = 1 ;

			/* highlight the appropriate menu item */
			writestring ( menu[choice - 1], sr + choice, sc + 1, 15 ) ;
		}
		else
		{
			if ( ascii == 13 )  /* Enter key */
				return ( choice ) ;

			if ( ascii == ESC )
			{
				displaymenuh ( mainmenu, 5 ) ;
				return ( ESC ) ;
			}

			hotkeychoice = 1 ;
			ascii = toupper ( ascii ) ;

			/* check whether hot key has been pressed */
			while ( *hotkeys != '\0' )
			{
				if ( *hotkeys == ascii )
					return ( hotkeychoice ) ;
				else
				{
					hotkeys++ ;
					hotkeychoice++ ;
				}
			}

			/* reset variable to point to the first hot key character */
			hotkeys = hotkeys - len ;
		}
	}
}

/* displays menu horizontally */
displaymenuh ( char **menu, int count )
{
	int col = 2, i ;

	size ( 32, 0 ) ;
	menubox ( 0, 0, 0, 79, 112, NO_SHADOW ) ;

	for ( i = 0 ; i < count ; i++ )
	{
		writestring ( menu[i], 0, col, 112 ) ;
		col = col + ( strlen ( menu[i] ) ) + 7 ;
	}

	size ( 5, 7 ) ;
}

/* receives user's choice for the horizontal menu displayed */
getresponseh ( char **menu, char *hotkeys, int count )
{
	int choice = 1, hotkeychoice, len, col ;

	size ( 32, 0 ) ;
	col = 2 ;

	/* calculate number of hot keys for the menu */
	len = strlen ( hotkeys ) ;

	/* highlight the first menu item */
	writestring ( menu[choice - 1], 0, col, 15 ) ;

	while ( 1 )
	{
		/* receive key */
		getkey() ;

		/* if special key is hit */
		if ( ascii == 0 )
		{
			switch ( scan )
			{
				case 77 :  /* right arrow key */

					/* make highlighted item normal */
					writestring ( menu[choice - 1], 0, col, 112 ) ;

					col += strlen ( menu[choice - 1] ) + 7 ;
					choice++ ;
					break ;

				case 75 :  /* left arrow key */

					/* make highlighted item normal */
					writestring ( menu[choice - 1], 0, col, 112 ) ;

					col -= ( strlen ( menu[choice - 2] ) + 7 ) ;
					choice-- ;
					break ;

				case 59 :  /* function key F1 for help */
					if ( choice == 1 )
						displayhelp ( 1 ) ;
					else
						displayhelp ( choice + 1 ) ;
			}

			/* if highlighted bar is on the first item and left arrow key is hit */
			if ( choice == 0 )
			{
				choice = count ;
				col = 65 ;
			}

			/* if highlghted bar is on the last item and right arrow key is hit */
			if ( choice > count )
			{
				choice = 1 ;
				col = 2 ;
			}

			/* highlight the appropriate menu item */
			writestring ( menu[choice - 1], 0, col, 15 ) ;
		}
		else
		{
			if ( ascii == 13 )  /* Enter key */
			{
				size ( 5, 7 ) ;
				return ( choice ) ;
			}

			if ( ascii == ESC )  /* Esc key */
			{
				/* make highlighted item normal */
				writestring ( menu[choice - 1], 0, col, 112 ) ;

				size ( 5, 7 ) ;
				return ( ESC ) ;
			}

			hotkeychoice = 1 ;
			ascii = toupper ( ascii ) ;

			/* check whether hot key has been pressed */
			while ( *hotkeys != '\0' )
			{
				if ( *hotkeys == ascii )
				{
					size ( 5, 7 ) ;
					return ( hotkeychoice ) ;
				}
				else
				{
					hotkeys++ ;
					hotkeychoice++ ;
				}
			}

			/* reset variable to point to the first hot key character */
			hotkeys = hotkeys - len ;
		}
	}
}

/* displays context sensitive help */
displayhelp ( int index )
{
	char *p ;
	int areareqd, i, row ;

	/* help messages */
	char *help[] = {
					"Cursor Movement Commands                          ",
					"--------------------------------------------------",
					"Character left                 Left arrow         ",
					"Character right                Right arrow        ",
					"Word left                      Ctrl - Left arrow  ",
					"Word right                     Ctrl - Right arrow ",
					"Line up                        Up arrow           ",
					"Line down                      Down arrow         ",
					"Page up                        Pgup               ",
					"",
					"Cursor Movement Commands   ( Contd. )              ",
					"--------------------------------------------------",
					"Page down                      PgDn               ",
					"Start of file                  Ctrl - Pgup        ",
					"End of file                    Ctrl - PgDn        ",
					"Top of screen                  Ctrl - Home        ",
					"Bottom of screen               Ctrl - End         ",
					"Start of line                  Home               ",
					"End of line                    End                ",
					"",
					"File Commands",
					"-------------------------------------------",
					"Load a file                      Alt - F L",
					"Pick recently edited file        Alt - F P",
					"Open new file                    Alt - F N",
					"Save and resume                  Alt - F S",
					"Save under another name          Alt - F A",
					"Merge another file               Alt - F M",
					"Change directory                 Alt - F C",
					"Output to printer                Alt - F O",
					"Search Commands",
					"-----------------------------------------",
					"Find                           Alt - S F",
					"Find & replace                 Alt - S R",
					"Repeat last search             Alt - S L",
					"Abort search                   Alt - S A",
					"Go to line number              Alt - S G",
					"",
					"",
					"",
					"Insert & Delete Commands",
					"-------------------------------------------------",
					"Insert mode on/off                     Ins",
					"Delete line                            Ctrl - Y",
					"Delete character to left of cursor     Backspace",
					"Delete character at cursor             Del",
					"Delete word to right of cursor         Ctrl - T",
					"",
					"",
					"",
					"Exit Commands",
					"----------------------------------------------",
					"Permanent exit to DOS      Alt - X, Alt - E E",
					"Temporary exit to DOS      Alt - E S",
					"",
					"",
					"",
					"",
					"",
					"",
					"Miscellaneous Commands",
					"------------------------------------------",
					"Main menu                          F10",
					"Take cursor to next tab stop       Tab",
					"Help                               F1",
					"Save                               F2",
					"Display product information        Sh-F10",
					"Exit from Easyedit                 Alt-X",
					"",
					""
				  } ;

	/* specific help messages for file menu */
	char *filehlp[] = {
						"                        Load",
						"                    ( Alt - F L )",
						"",
						"Loads  or  creates  a  file.  You  have to type in",
						"the  name of the file. If the file does not exist,",
						"you have the option of creating a new file of that",
						"name.",
						"                        Pick",
						"                    ( Alt - F P )",
						"",
						"Lets you pick a file from the \"pick list\", which",
						"is  a list of the five most recently edited files.",
						"Selecting a file from the list loads it in memory.",
						"",
						"                        New",
						"                    ( Alt - F N )",
						"",
						"Allows you to create a file called NONAME and work",
						"with  it.  While saving the file, you are prompted",
						"to rename it.",
						"",
						"                        Save",
						"                    ( Alt - F S )",
						"",
						"Saves  the  current  file  to disk. If the current",
						"file  is  NONAME,  you  are  asked  to  rename it.",
						"Pressing  F2  from anywhere in the system does the",
						"same thing.",
						"                      Save As",
						"                   ( Alt - F A )",
						"",
						"Saves  the current file contents under a new name.",
						"The  original  file  contents remain intact on the",
						"disk. The new file now becomes the current file.",
						"",
						"                       Merge",
						"                   ( Alt - F M )",
						"",
						"Merges  the  contents  of  a file on disk into the",
						"currently  loaded file at current cursor location.",
						"If the file to be merged does not exist, a warning",
						"is issued.",
						"                    Change Dir",
						"                   ( Alt - F C )",
						"",
						"Displays the current directory and lets you change",
						"to the drive and directory specified at the prompt",
						"",
						"",
						"                  Output to printer",
						"                   ( Alt - F O )",
						"",
						"Lets you specify a file to be printed. The margins",
						"and  the page length can be specified. If the file",
						"does not exist, a warning is issued.",
						""
					} ;

	/* specific help messages for exit menu */
	char *exithlp[] = {
					 "                       Exit",
					 "                   ( Alt - E E )",
					 "",
					 "Use  this  option to exit from Easyedit. If you've",
					 "Modified   a  workfile  without saving it, you are",
					 "prompted to do so now. The hot key Alt - X, allows",
					 "you to exit Easyedit from anywhere in the system.",
					 "                     OS Shell",
					 "                   ( Alt - E S )",
					 "",
					 "Leaves Easyedit temporarily & takes you to the DOS",
					 "prompt. To return to Easyedit, type \"exit\" & press",
					 "Enter at DOS prompt. Use this to run a DOS command",
					 "without quitting Easyedit.",
				  } ;

	/* calculate area required to save screen contents where help box is to be popped up */
	areareqd = ( 20 - 4 + 1 ) * ( 67 - 12 + 1 ) * 2 ;

	p = malloc ( areareqd ) ;  /* allocate memory */

	/* if memory allocation fails */
	if ( p == NULL )
		error_exit() ;

	/* save screen contents */
	savevideo ( 4, 12, 20, 67, p ) ;

	/* draw a filled box */
	menubox ( 4, 12, 20, 67, 112, 07 ) ;

	/* draw a double-lined box */
	drawbox ( 4, 12, 19, 65, 112 ) ;

	/* display `Help' in a box with shadow */
	menubox ( 4, 35, 5, 44, 32, HALF_SHADOW ) ;
	writestring ( "Help", 4, 37, 32 ) ;

	/* if one of the first six help screens is to be displayed */
	if ( index <= 6 )
	{
		/* calculate number of starting help message to be displayed */
		i = ( index - 1 ) * 10 ;

		row = 6 ;

		while ( 1 )
		{
			writestring ( help[i], row, 14, 112 ) ;
			i++ ;
			row++ ;

			if ( row > 15 )
			{
				/* display PgDn/PgUp/OK buttons depending on help screen number */
				switch ( i )
				{
					case 10 :  /* first help screen */

						menubox ( 17, 22, 18, 31, 32, HALF_SHADOW ) ;
						writestring ( "Pg$Dn", 17, 24, 32 ) ;
						menubox ( 17, 48, 18, 55, 32, HALF_SHADOW ) ;
						writestring ( "$OK", 17, 50, 32 ) ;
						break ;

					case 70 :  /* last help screen */

						menubox ( 17, 22, 18, 31, 32, HALF_SHADOW ) ;
						writestring ( "Pg$Up", 17, 24, 32 ) ;
						menubox ( 17, 48, 18, 55, 32, HALF_SHADOW ) ;
						writestring ( "$OK", 17, 50, 32 ) ;
						break ;

					default :  /* intermediate help screen */

						menubox ( 17, 20, 18, 29, 32, HALF_SHADOW ) ;
						writestring ( "Pg$Dn", 17, 22, 32 ) ;
						menubox ( 17, 35, 18, 44, 32, HALF_SHADOW ) ;
						writestring ( "Pg$Up", 17, 37, 32 ) ;
						menubox ( 17, 50, 18, 57, 32, HALF_SHADOW ) ;
						writestring ( "$OK", 17, 52, 32 ) ;
				}

				/* continue till either PgUp, PgDn, OK or Esc is hit */
				while ( 1 )
				{
					getkey() ;

					/* if Esc is hit or OK is selected */
					if ( ascii == 27 || ascii == 'o' || ascii == 'O' )
					{
						restorevideo ( 4, 12, 20, 67, p ) ;
						free ( p ) ;
						return ;
					}

					/* if PgDn is selected and it is not the last help screen */
					if ( ( scan == 81 || ascii == 'd' || ascii == 'D' ) && ( i != 70 ) )
					{
						menubox ( 6, 13, 18, 64, 112, 0 ) ;
						row = 6 ;
						break ;
					}

					/* if PgUp is selected and it is not the first help screen */
					if ( ( scan == 73 || ascii == 'U' || ascii == 'u' ) && i != 10 )
					{
						i -= 20 ;  /* go back by 20 lines */
						menubox ( 6, 13, 18, 64, 112, 0 ) ;
						row = 6 ;
						break ;
					}
				}
			}
		}
	}
	else
	{
		/* if specific file help is to be displayed */
		if ( index <= 15 )
		{
			/* calculate number of starting help message to be displayed */
			i = ( index - 8 ) * 7 ;

			/* display help screen */
			for ( row = 7 ; row < 14 ; row++ )
			{
				writestring ( filehlp[i], row, 14, 112 ) ;
				i++ ;
			}
		}
		else
		{
			/* calculate number of starting help message to be displayed */
			i = ( index - 16 ) * 7 ;

			/* display help screen */
			for ( row = 7 ; row < 14 ; row++ )
			{
				writestring ( exithlp[i], row, 14, 112 ) ;
				i++ ;
			}
		}

		/* display OK button */
		menubox ( 15, 34, 16, 43, 32, HALF_SHADOW ) ;
		writestring ( " $OK ", 15, 36, 32 ) ;

		/* continue till Esc is hit or OK is selected */
		while ( 1 )
		{
			getkey() ;

			if ( ascii == 27 || ascii == 'o' || ascii == 'O' )
			{
				restorevideo ( 4, 12, 20, 67, p ) ;
				free ( p ) ;
				break ;
			}
		}
	}
}