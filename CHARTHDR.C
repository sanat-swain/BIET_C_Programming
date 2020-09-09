/*  CHARTHDR.C  */
# include "graphics.h"
# include "dos.h"
# include "math.h"
# include "process.h"
# include "alloc.h"
# include "stdlib.h"
# include "stdio.h"
# include "fcntl.h"
# include "ctype.h"
# include "types.h"
# include "stat.h"

# define CGA 1
# define ENTRYOVER 1
# define ENTRYSTART 0
# define ESCAPE 27
# define END 79
# define LINE 1
# define XY 2

/* various menu definitions */
/* character following ^ symbol is the hot key */
char *mainmenu[] =	{
						" ^Data ",
						" ^Graph ",
						" ^Files ",
						" ^Exit "
					} ;

char *datamenu[] =	{
						" ^File ",
						" ^Keyboard ",
						" ^Return "
					} ;

char *graphmenu[] =	{
						" ^Bar chart ",
						" ^Stacked bar ",
						" ^Pie chart ",
						" ^X Y chart ",
						" ^Line chart ",
						" ^Return "
					} ;

char *filesmenu[] = {
						" ^Load ",
						" ^Save ",
						" ^Print ",
						" ^Return "
				    } ;

char *keyboardmenu[] = 	{
							" ^New data ",
							" ^Edit data ",
							" ^Legend ",
							" ^Return "
						} ;

char *exitmenu[] =	{
						" ^Exit ",
						" ^Shell ",
						" ^Return "
					} ;

char *messages[] = 	{
					 	"Main Menu",
						"Allows entry of data from file / keyboard",
						"Draws bar / stacked / X Y / line / pie chart",
						"Loads / Saves / Prints a graph",
						"Exit to DOS",
						"Data Menu",
						"Reads data from a file",
						"Entry of data from keyboard",
						"Return to main menu",
						"Graph Menu",
						"Draws bar chart",
						"Draws stacked bar chart",
						"Draws pie chart",
						"Draws X Y chart",
						"Draws line chart",
						"Return to main menu",
						"File Menu",
						"Loads a graph",
						"Saves a graph",
						"Prints a graph",
						"Return to main menu",
						"Keyboard Menu",
						"Enter fresh data",
						"Edit current data",
						"Entry of legend",
						"Return to Data menu",
						"Exit Menu",
						"Exit to DOS",
						"OS Shell",
						"Return to main menu",
						"   Chart Master   ",
						"Memory allocation error",
						"Press any key to continue...",
					} ;

int count, ascii, scan, data_type, max_x_entries, max_y_sets ;
int area, gd, gm ;
long starttime, endtime ;
char y_sets_desc[6][31], titles[3][31], filename[31], arr[] = "ABCDEF" ;
char  *graph_ptr ;
char far *vid_mem ;
double yrange[15][6], xrange[15] ;

/* symbols used for indicating points while drawing XY/Line chart */
char *symbol[] = { "+", "*", "Û", "O", "³", "#" } ;

/* pops up a menu on the existing screen contents */
popupmenu ( char **menu, int count, char *hotkeys, int helpnumber )
{
	int sr, sc, er, ec, srmenu, scmenu, i, l = 0, len, areareqd, choice ;
	char *p ;

	/* calculate starting row and ending row of menu box */
	sr = ( 25 - ( count + 10 ) ) / 2 ;
	er = 24 - sr ;
	if ( count % 2 )
		er++ ;

	/* find the longest help message */
	for ( i = 1 ; i <= count ; i++ )
	{
		len = strlen ( messages[helpnumber + i] ) ;
		if ( len > l )
			l = len ;
	}

	/* calculate starting column of menu box depending on longest help message */
	sc = ( 80 - ( l + 9 ) ) / 2 ;

	/* calculate ending column of menu box */
	ec = 80 - sc ;

	/* calculate area required to save screen contents where menu is to be popped up */
	areareqd = ( er - sr + 1 ) * ( ec - sc + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	/* save screen contents into allocated memory */
	savevideo ( sr, sc, er, ec, p ) ;

	/* give the effect of zooming menus */
	delay ( 15 ) ;
	menubox ( 9, 37, 15, 42, 127, 0 ) ;
	drawbox ( 9, 37, 15, 42, 127 ) ;
	delay ( 15 ) ;
	menubox ( sr, sc, er, ec, 127, 07 ) ;
	drawbox ( sr + 1, sc + 2, er - 2, ec - 4, 127 ) ;

	/* determine length of current menu name */
	l = strlen ( messages[helpnumber] ) ;

	/* calculate column from where the menu name is to be displayed */
	l = 39 - l / 2 ;

	/* display menu name */
	writestring ( messages[helpnumber], sr + 2, l, 127 ) ;

	/* draw two horizontal lines */
	for ( i = sc + 3 ; i <= ec - 4 ; i++ )
	{
		writechar ( sr + 3, i, 196, 127 ) ;
		writechar ( er - 4, i, 196, 127 ) ;
	}
	/* display corner characters */
	writechar ( sr + 3, sc + 2, 199, 127 ) ;
	writechar ( er - 4, sc + 2, 199, 127 ) ;
	writechar ( sr + 3, ec - 4, 182, 127 ) ;
	writechar ( er - 4, ec - 4, 182, 127 ) ;

	/* find the longest menu item in current menu */
	l = 0 ;
	for ( i = 0 ; i < count ; i++ )
	{
		len = strlen ( menu[i] ) ;
		if ( len > l )
			l = len ;
	}

	/* calculate starting row and starting column for displaying menu items */
	srmenu = ( ( 25 - count ) / 2 ) - 1 ;
	scmenu = ( ( 80 - ( l + 6 ) ) / 2 ) - 1 ;

	/* display the menu in the menu box drawn earlier */
	displaymenu ( menu, count, srmenu, scmenu ) ;

	/* display help message */
	writestring ( messages[helpnumber + 1], er - 3, sc + 4, 127 ) ;

	/* receive user's choice */
	choice = getresponse ( menu, hotkeys, srmenu, scmenu, count, helpnumber + 1, er, sc ) ;

	/* restore original screen contents */
	restorevideo ( sr, sc, er, ec, p ) ;

	/* free allocated memory */
	free ( p ) ;

	return ( choice ) ;
}

/* writes a character and its attribute in VDU memory */
writechar ( int r, int c, char ch, int attb )
{
	char far *v ;

	/* calculate address in VDU memory corresponding to row r and column c */
	v = vid_mem + r * 160 + c * 2 ;
	*v = ch ;  /* store character */
	v++ ;
	*v = attb ;  /* store attribute */
}

/* writes a string into VDU memory in the desired attribute */
writestring ( char *s, int r, int c, int attb )
{
	while ( *s != '\0' )
	{
		/* if the next character is a hot key, write in different attribute, otherwise in normal attribute */
		if ( *s == '^' )
		{
			s++ ;
			writechar ( r, c, *s, 113 ) ;
		}
		else
			writechar ( r, c, *s, attb ) ;
		s++ ;
		c++ ;
	}
}

/* saves screen contents into allocated memory in RAM */
savevideo ( int sr, int sc, int er, int ec, char *buffer )
{
	char far *v ;
	int i, j ;

	for ( i = sr ; i <= er ; i++ )
	{
		for ( j = sc ; j <= ec ; j++ )
		{
			/* calculate address in VDU memory corresponding to row r and column c */
			v = vid_mem + i * 160 + j * 2 ;
			*buffer = *v ;  /* store character */
			v++ ;
			buffer++ ;
			*buffer = *v ;  /* store attribute */
			buffer++ ;
		}
	}
}

/* restores screen contents from allocated memory in RAM */
restorevideo ( int sr, int sc, int er, int ec, char *buffer )
{
	char far *v ;
	int i, j ;

	for ( i = sr ; i <= er ; i++ )
	{
		for ( j = sc ; j <= ec ; j++ )
		{
			/* calculate address in VDU memory corresponding to row r and column c */
			v = vid_mem + i * 160 + j * 2 ;
			*v = *buffer ;  /* restore character */
			v++ ;
			buffer++ ;
			*v = *buffer ;  /* restore attribute */
			buffer++ ;
		}
	}
}

/* draws a filled box with or without shadow */
menubox ( int sr, int sc, int er, int ec, int fill, int shadow )
{
	int i, j ;

	/* draw filled box */
	for ( i = sr ; i < er ; i++ )
		for ( j = sc ; j < ( ec - 1 ) ; j++ )
			writechar ( i, j, ' ', fill ) ;

	/* if shadow is desired */
	if ( shadow != 0 )
	{
		/* draw a column with different attribute */
		for ( i = sr + 1 ; i <= er ; i++ )
		{
			writechar ( i, ec, ' ', shadow ) ;
			writechar ( i, ec - 1, ' ', shadow ) ;
		}

		/* draw a row with different attribute */
		for ( j = sc + 2 ; j <= ec ; j++ )
			writechar ( er, j, ' ', shadow ) ;
	}
	else
	{
		/* draw a column with same attribute as menu box */
		for ( i = sr ; i <= er ; i++ )
		{
			writechar ( i, ec, ' ', fill ) ;
			writechar ( i, ec - 1, ' ', fill ) ;
		}

		/* draw a row with same attribute as menu box */
		for ( j = sc ; j <= ec ; j++ )
			writechar ( er, j, ' ', fill ) ;
	}
}

/* displays the menu in the box drawn by menubox() */
displaymenu ( char **menu, int count, int sr, int sc )
{
	int i ;

	for ( i = 0 ; i < count ; i++ )
	{
		/* write menu item in VDU memory */
		writestring ( menu[i], sr + 1, sc + 1, 127 ) ;
		sr++ ;
	}
}

/* draws a double lined box */
drawbox ( int sr, int sc, int er, int ec, int attr )
{
	int i ;

	/* draw vertical lines */
	for ( i = sc + 1 ; i < ec ; i++ )
	{
		writechar ( sr, i , 205, attr ) ;
		writechar ( er, i, 205, attr ) ;
	}

	/* draw horizontal lines */
	for ( i = sr + 1 ; i < er ; i++ )
	{
		writechar ( i, sc, 186, attr ) ;
		writechar ( i, ec, 186, attr ) ;
	}

	/* draw four corners */
	writechar ( sr, ec, 187, attr ) ;
	writechar ( er, sc, 200, attr ) ;
	writechar ( er, ec, 188, attr ) ;
	writechar ( sr, sc, 201, attr ) ;
}

/* gets user's response for the current menu */
getresponse ( char **menu, char *hotkeys, int sr, int sc, int count, int helpnumber, int ermenu, int scmenu )
{
	int choice = 1, len, hotkeychoice, i, l = 0 ;

	/* find the longest help message */
	for ( i = 0 ; i < count ; i++ )
	{
		len = strlen ( messages[helpnumber + i] ) ;
		if ( len > l )
			l = len ;
	}

	/* highlight first menu item */
	writestring ( menu[choice - 1], sr + choice, sc + 1, 15 ) ;

	while ( 1 )
	{
		getkey() ;  /* receive key */

		/* if special key is hit */
		if ( ascii == 0 )
		{
			switch ( scan )
			{
				case 80 :  /* down arrow key */

					/* make highlighted item normal */
					writestring ( menu[choice - 1], sr + choice, sc + 1, 127 ) ;

					choice++ ;
					helpnumber++ ;
					break ;

				case 72 :  /* up arrow key */

					/* make highlighted item normal */
					writestring ( menu[choice - 1], sr + choice, sc + 1, 127 ) ;

					choice-- ;
					helpnumber-- ;
					break ;

				default :
					break ;
			}

			/* if highlighted bar is on first item and up arrow key is hit */
			if ( choice == 0 )
			{
				choice = count ;
				helpnumber = helpnumber + count ;
			}

			/* if highlighted bar is on last item and down arrow key is hit */
			if ( choice > count )
			{
				choice = 1 ;
				helpnumber = helpnumber - count ;
			}

			/* highlight the appropriate menu item */
			writestring ( menu[choice - 1], sr + choice, sc + 1, 15 ) ;

			/* clear previous help message */
			for ( i = scmenu + 4 ; i < ( scmenu + 4 + l ) ; i++ )
				writechar ( ermenu - 3, i, ' ', 127 ) ;

			/* write the corresponding help message */
			writestring ( messages[helpnumber], ermenu - 3, scmenu + 4, 127 ) ;
		}
		else
		{
			if ( ascii == 13 )  /* Enter key */
				return ( choice ) ;

			ascii = toupper ( ascii ) ;
			hotkeychoice = 1 ;
			len = strlen ( hotkeys ) ;

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

/* gets ascii and scan codes of key pressed */
getkey()
{
	union REGS i, o ;

	/* wait till a key is hit */
	while ( !kbhit() )
		;

	i.h.ah = 0 ;  /* service number */

	/* issue interrupt */
	int86 ( 22, &i, &o ) ;

	ascii = o.h.al ;
	scan = o.h.ah ;
}

/* displays or hides the cursor */
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
