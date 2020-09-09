/* Refer Appendix B in the book if you want to create standalone
   EXE file of this program during compilation */

# include "conio.h"
# include "stdlib.h"
# include "dos.h"
# include "stdio.h"
# include "graphics.h"

# define RIGHT 0
# define LEFT 1
# define UP 2
# define DOWN 3

int maze[25][80], score, row, col, ascii, scan, liveslost, delayfactor ;
int gd = CGA, gm = CGAHI, midx, midy, maxx, maxy ;
int bugnumber, r[5], c[5], dir[5] ;
unsigned char charbelow[5] ;
char far *vid_mem = ( char far * ) 0xB8000000L ;

main()
{
	char ans ;

	/* initialise random number generator with a random value */
	randomize() ;

	while ( 1 )
	{
		/* initialise variables at the start of each game */
		initialise() ;

		/* create opening screen */
		startscreen() ;

		/* draw the screen for the game */
		gamescreen() ;

		/* monitor the movement of the bugs and the Eater */
		monitor() ;

		/* create ending screen */
		ans = endscreen() ;

		/* check whether the user wishes to continue playing */
		if ( ans == 'N' )
			break ;
	}
}

/* creates opening screen */
startscreen()
{
	char ch ;
	int i ;

	/* initialise the graphics system */
	initgraph ( &gd, &gm, "c:\\tc\\bgi" ) ;

	/* get maximum x and y screen coordinates */
	maxx = getmaxx() ;
	maxy = getmaxy() ;

	/* calculate the center of the screen */
	midx = maxx / 2 ;
	midy = maxy / 2 ;

	/* draw a double-lined box */
	rectangle ( 0, 0, maxx, maxy ) ;
	rectangle ( 2, 2, maxx - 2, maxy - 2 ) ;

	/* draw two vertical lines */
	line ( 55, 1, 55, maxy - 2 ) ;
	line ( maxx - 55, 1, maxx - 55, maxy - 2 ) ;

	/* display the string "EATER", horizontally */
	settextjustify ( CENTER_TEXT, CENTER_TEXT ) ;
	settextstyle ( DEFAULT_FONT, HORIZ_DIR, 6 ) ;
	outtextxy ( midx, midy, "EATER" ) ;

	/* display the string "Eater" vertically on both sides */
	settextstyle ( DEFAULT_FONT, VERT_DIR, 4 ) ;
	outtextxy ( 30, midy, "Eater" ) ;
	outtextxy ( maxx - 30, midy, "Eater" ) ;

	/* place the Eater character at random on the start screen */
	gotoxy ( 10, 7 ) ;
	printf ( "%c", 2 ) ;
	gotoxy ( 35, 17 ) ;
	printf ( "%c", 2 ) ;
	gotoxy ( 55, 3 ) ;
	printf ( "%c", 2 ) ;
	gotoxy ( 65, 23 ) ;
	printf ( "%c", 2 ) ;
	gotoxy ( 70, 8 ) ;
	printf ( "%c", 2 ) ;
	gotoxy ( 20, 22 ) ;
	printf ( "%c", 2 ) ;
	gotoxy ( 15, 12 ) ;
	printf ( "%c", 2 ) ;
	gotoxy ( 30, 5 ) ;
	printf ( "%c", 2 ) ;
	gotoxy ( 68, 18 ) ;
	printf ( "%c", 2 ) ;

	/* play the starting music */
	music ( 1 ) ;

	/* clear the area enclosed by the double-lined boundary */
	setviewport ( 1, 1, maxx - 1, maxy - 1, 1 ) ;
	clearviewport() ;

	/* draw the screen for displaying instructions */
	rectangle ( 0 + 30, 0, maxx - 30, maxy ) ;
	line ( 33, 0, 33, maxy ) ;
	line ( maxx - 33, 0, maxx - 33, maxy ) ;

	for ( i = 15 ; i <= maxy - 15 ; i+= 15 )
	{
		ellipse ( 15, i, 0, 360, 6, 3 ) ;
		ellipse ( maxx - 15, i, 0, 360, 6, 3 ) ;
	}

	settextjustify ( CENTER_TEXT, TOP_TEXT ) ;
	settextstyle ( DEFAULT_FONT, HORIZ_DIR, 3 ) ;
	outtextxy ( midx, 10, "Instructions" ) ;

	/* display instructions */
	settextstyle ( DEFAULT_FONT, HORIZ_DIR, 0 ) ;
	outtextxy ( midx, 40, "Your goal :- To eat up all the 400 crunchy-munchy  " ) ;
	outtextxy ( midx, 50, "             titbits scattered throughout the maze." ) ;
	outtextxy ( midx, 70, "Your task is not easy!  There are 5 bugs chasing   " ) ;
	outtextxy ( midx, 80, "the Eater. You will have to watch out for them.    " ) ;
	outtextxy ( midx, 100, "To help you,  we have bestowed the Eater with 3    " ) ;
	outtextxy ( midx, 110, "lives.                                             " ) ;
	outtextxy ( midx, 130, "You can move around the maze using arrow keys.     " ) ;
	outtextxy ( midx, 150, "If you are ready to start, press a key.            " ) ;
	settextstyle ( DEFAULT_FONT, HORIZ_DIR, 3 ) ;
	outtextxy ( midx, 170, "We wish you luck!" ) ;

	/* wait for key press */
	while ( !kbhit() )
		;

	/* flush the keyboard buffer */
	if ( getch() == 0 )
		getch() ;

	/* draw the screen for asking user level */
	setviewport ( 0, 0, maxx, maxy, 1 ) ;
	clearviewport() ;
	rectangle ( midx - 200, midy - 60, midx + 200, midy + 60 ) ;

	settextstyle ( DEFAULT_FONT, HORIZ_DIR, 2 ) ;
	outtextxy ( midx, 60, "Select Speed:" ) ;

	settextstyle ( DEFAULT_FONT, HORIZ_DIR, 1 ) ;
	outtextxy ( midx, 90, "Slow ( S )    " ) ;
	outtextxy ( midx, 105, "Medium ( M )  " ) ;
	outtextxy ( midx, 120, "Fast ( F )    " ) ;

	/* get user level */
	while ( !kbhit() )
		;

	/* flush the keyboard buffer */
	if ( ( ch = getch() ) == 0 )
		getch() ;

	/* change over to text mode */
	closegraph() ;
	restorecrtmode() ;

	/* set the value of variable delayfactor according to level selected */
	switch ( toupper ( ch ) )
	{
		case 'S' :
			delayfactor = 90 ;
			break ;

		case 'M' :
			delayfactor = 40 ;
			break ;

		case 'F' :
			delayfactor = 20 ;
			break ;

		default :
			delayfactor = 90 ;
	}
}

/* draws the screen for the game */
gamescreen()
{
	int i ;

	size ( 32, 0 ) ;  /* hide cursor */

	/* fill the screen with the specified color */
	drawbox ( 0, 0, 24, 79, 39 ) ;

	/* draw horizontal lines of the maze */
	for ( i = 1 ; i <= 21 ; i = i + 4 )
		hline ( 219, 15, i, 1, 79 ) ;

	/* draw vertical lines of the maze */
	for ( i = 1 ; i <= 79 ; i = i + 6 )
		vline ( 219, 15, i, 1, 21 ) ;

	/* draw the rows of titbits */
	hline ( 250, 63, 3, 4, 22 ) ;
	hline ( 250, 63, 3, 28, 57 ) ;
	hline ( 250, 63, 3, 59, 76 ) ;
	hline ( 250, 63, 7, 10, 22 ) ;
	hline ( 250, 63, 7, 34, 76 ) ;
	hline ( 250, 63, 11, 4, 30 ) ;
	hline ( 250, 63, 11, 32, 52 ) ;
	hline ( 250, 63, 11, 64, 64 ) ;
	hline ( 250, 63, 11, 70, 76 ) ;
	hline ( 250, 63, 15, 10, 22 ) ;
	hline ( 250, 63, 15, 4, 10 ) ;
	hline ( 250, 63, 15, 28, 40 ) ;
	hline ( 250, 63, 15, 64, 70 ) ;
	hline ( 250, 63, 19, 10, 46 ) ;
	hline ( 250, 63, 19, 52, 58 ) ;
	hline ( 250, 63, 19, 70, 76 ) ;
	hline ( 250, 63, 19, 59, 63 ) ;
	hline ( 250, 63, 11, 58, 67 ) ;

	/* draw the columns of titbits */
	vline ( 250, 63, 4, 4, 19 ) ;
	vline ( 250, 63, 10, 4, 6 ) ;
	vline ( 250, 63, 10, 16, 18 ) ;
	vline ( 250, 63, 22, 4, 6 ) ;
	vline ( 250, 63, 22, 12, 14 ) ;
	vline ( 250, 63, 22, 4, 10 ) ;
	vline ( 250, 63, 28, 4, 10 ) ;
	vline ( 250, 63, 28, 16, 18 ) ;
	vline ( 250, 63, 40, 8, 14 ) ;
	vline ( 250, 63, 46, 12, 18 ) ;
	vline ( 250, 63, 52, 12, 18 ) ;
	vline ( 250, 63, 58, 8, 14 ) ;
	vline ( 250, 63, 58, 16, 18 ) ;
	vline ( 250, 63, 64, 8, 19 ) ;
	vline ( 250, 63, 70, 8, 10 ) ;
	vline ( 250, 63, 70, 16, 18 ) ;
	vline ( 250, 63, 76, 4, 6 ) ;
	vline ( 250, 63, 76, 12, 18 ) ;
	vline ( 250, 63, 34, 4, 16 ) ;
	vline ( 250, 63, 34, 18, 19 ) ;
	vline ( 250, 63, 46, 4, 6 ) ;
	vline ( 250, 63, 58, 4, 6 ) ;
	vline ( 250, 63, 16, 9, 17 ) ;

	gotoxy ( 51, 24 ) ;
	printf ( "Press Esc to stop the game" ) ;
	gotoxy ( 2, 24 ) ;
	printf ( "Points: %3d", score ) ;

	/* place the five bugs at strategic positions */
	writechar ( 2, 3, 76, 112 ) ;
	writechar ( 2, 3, 22, 112 ) ;
	writechar ( 2, 11, 5, 112 ) ;
	writechar ( 2, 19, 63, 112 ) ;
	writechar ( 2, 19, 46, 112 ) ;

	/* play music */
	music ( 2 ) ;
}

/* draws a box filling the required area */
drawbox ( int sr, int sc, int er, int ec, char attr )
{
	int r, c ;
	char far *v ;

	for ( r = sr ; r <= er ; r++ )
	{
		for ( c = sc ; c <= ec ; c++ )
		{
			/* calculate address */
			v = vid_mem + ( r * 160 ) + ( c * 2 ) ;

			*v = ' ' ;  /* write a space */
			v++ ;
			*v = attr ;  /* write attribute */
		}
	}
}

/* writes character and its attribute into VDU memory */
writechar ( char ch, int r, int c, int attr )
{
	char far *v ;

	/* calculate address */
	v = vid_mem + ( r * 160 ) + c * 2 ;

	*v = ch ;  /* store ascii value */
	v++ ;
	*v = attr ;  /* store attribute */
}

/* draws horizontal line */
hline ( unsigned char ch, int attr, int r, int c1, int c2 )
{
	int c ;

	for ( c = c1 ; c <= c2 ; c++ )
	{
		writechar ( ch, r, c, attr ) ;

		/* if titbit placed at row r and column c, set corresponding element of array maze[][] to 1 */
		if ( ch == 250 )
			maze[r][c] = 1 ;
	}
}

/* draws vertical line */
vline ( unsigned char ch, int attr, int c, int r1, int r2 )
{
	int r ;

	for ( r = r1 ; r <= r2 ; r++ )
	{
		writechar ( ch, r, c, attr ) ;

		/* if titbit placed at row r and column c, set corresponding element of array maze[][] to 1 */
		if ( ch == 250 )
			maze[r][c] = 1 ;
	}
}

/* monitors the movement of the bugs and the Eater */
monitor()
{
	int key ;
	unsigned char ch ;

	while ( 1 )
	{
		/* place Eater at specified row and column */
		writechar ( 1, row, col, 112 ) ;

		/* move bugs around until a key is hit */
		getkeyhit() ;

		/* if all 3 lives of the Eater are lost */
		if ( liveslost == 3 )
			break ;

		/* place a space in the position currently occupied by the Eater */
		writechar ( ' ', row, col, 63 ) ;

		/* update the position of the Eater according to the key pressed */
		key = testkeys() ;

		/* if invalid key pressed */
		if ( key == 0 )
		{
			/* write back Eater in its original position */
			writechar ( 1, row, col, 112 ) ;
		}
		else
		{
			/* read character at the position which the Eater is to occupy */
			readchar ( row, col, &ch ) ;

			/* if character read is titbit, increment score and sound music */
			if ( ch == 250 )
			{
				score++ ;
				music ( 3 ) ;
			}

			/* if character read is bug, kill the Eater */
			if ( ch == 2 )
				killeater() ;

			/* if all three lives of the Eater are lost */
			if ( liveslost == 3 )
				break ;

			/* print the latest score */
			gotoxy ( 2, 24 ) ;
			printf ( "Points: %3d", score ) ;

			/* if all the titbits are eaten up */
			if ( score >= 400 )
			{
				/* erase the last titbit */
				writechar ( ' ', row, col, 63 ) ;

				break ;
			}
		}
	}
}

/* moves bugs around until a key is hit */
getkeyhit()
{
	union REGS i, o ;

	/* until the player hits a key, move each bug in turn */
	while ( !kbhit() )
	{
		/* introduce delay */
		delay ( delayfactor ) ;

		/* move the bug one step in the appropriate direction */
		bug ( &r[bugnumber], &c[bugnumber], &dir[bugnumber],
				&charbelow[bugnumber] ) ;

		/* if all three lives of the Eater are lost */
		if ( liveslost == 3 )
			return ;

		/* go to the next bug */
		bugnumber++ ;

		/* start with the first bug if all five bugs have been moved */
		if ( bugnumber == 5 )
			bugnumber = 0 ;
	}

	/* issue interrupt to read the ascii and scan codes of the key pressed */
	i.h.ah = 0 ;  /* store service number */
	int86 ( 22, &i, &o ) ;  /* issue interrupt */
	ascii = o.h.al ;
	scan = o.h.ah ;
}

/* reports which key has been hit */
testkeys()
{
	switch ( scan )
	{
		case 72 :  /* up arrow */

			/* if path is not present in the specified direction */
			if ( maze[row - 1][col] != 1 )
				return ( 0 ) ;

			/* update row of Eater */
			row-- ;
			break ;

		case 80 :  /* down arrow */

			if ( maze[row + 1][col] != 1 )
				return ( 0 ) ;

			row++ ;
			break ;

		case 77 :  /* right arrow */

			/* if path is not present in the specified direction */
			if ( maze[row][col + 1] != 1 )
				return ( 0 ) ;

			/* update column of Eater */
			col++ ;
			break ;

		case 75 :  /* left arrow */

			if ( maze[row][col - 1] != 1 )
				return ( 0 ) ;

			col-- ;
			break ;

		case 1 :  /* Esc key */

			exit ( 0 ) ;  /* terminate program */

		default :
			return ( 0 ) ;
	}
}

/* reads the character present at row r and column c into ch */
readchar ( int r, int c, unsigned char *ch )
{
	char far *v ;

	/* calculate address */
	v = vid_mem + ( r * 160 ) + c * 2 ;

	*ch = *v ;
}

/* moves the specified bug in the appropriate direction */
bug ( int *r, int *c, int *dir, unsigned char *ch )
{
	int trials = 1, flag = 0 ;
	char temp ;

	/* select a valid direction which takes the bug closer to Eater */
	/* in each if statement the 1st condition ch` OÄàÉàœÅÉàOÄ  à  ÅœÇ œÅÉéœâÇàÇ  œÄ  âœÄ  àOÅÉàOåÄOåÄœÅÄOÅÉàOòà¡œÄààOÅÉàOƒÄâœå  âÇÇ  œåà OÇœÅÉàOÄze permits a movement in that direction */
	if ( abs ( *r - 1 - row ) < abs ( *r - row ) && maze[*r- 1][*c] == 1 )
		*dir = UP ;
	else
		if ( abs ( *r + 1 - row ) < abs ( *r - row ) && maze[*r + 1][*c] == 1 )
			*dir = DOWN ;
		else
			if ( abs ( *c + 1 - col ) < abs ( *c - col ) && maze[*r][*c + 1] == 1 )
				*dir = RIGHT ;
			else
				if ( abs( *c - 1 - col ) < abs( *c - col ) && maze[*r][*c - 1] == 1 )
					*dir = LEFT ;

	/* check whether the direction chosen contains another bug, if so find an alternate direction */
	while ( 1 )
	{
		switch ( *dir )
		{
			case RIGHT :

				/* if there is a path to the right of bug */
				if ( maze[*r][*c + 1] == 1 )
				{
					/* read the character to the right of the bug */
					readchar ( *r, *c + 1, &temp ) ;

					/* if the character is again a bug find alternate direction */
					if ( temp == 2 )
					{
						/* if path exists to the left of bug */
						if ( maze[*r][*c - 1] == 1 )
							*dir = LEFT ;
						else
						{
							/* if path exists to the top of bug */
							if ( maze[*r - 1][*c] == 1 )
								*dir = UP ;
							else
							{
								/* if path exists below the bug */
								if ( maze[*r + 1][*c] == 1 )
									*dir = DOWN ;
							}
						}
					}
					else
					{
						/* if there is no bug to the right of the bug being considered, move the bug right */
						movebugright ( r, c, ch ) ;
						flag = 1 ;
					}
				}
				else
				{
					/* since there is no path in the RIGHT direction, try another path */
					*dir = random ( 4 ) ;
				}

				break ;

			case LEFT :

				/* if there is a path to the left of bug */
				if ( maze[*r][*c - 1] == 1 )
				{
					/* read the character to the left of the bug */
					readchar ( *r, *c - 1#üë üÑüÄ‡·‡·‰‰‰‰‰ÄÅüüü		üüüü üü	ü	 üÅÄ‡·‰‰‰‰‰üáü üÄÄüâüÑ‡·‰‰‰‰‰ ‡·‰‰‰‰‰‰ÄÅüüüü üü	ü ü üÅÄ‡·‰‰‰‰‰‰üáü  Å	  ÅüÄüå üÄÄüåüÑ‡·‰‰‰‰‰‰‰Å	üÄü	$0'üÄ‡·‰‰È			else
						{
							/* if path exists to the top of bug */
							if ( maze[*r - 1][*c] == 1 )
								*dir = UP ;
							else
							{
								/* if path exists below the bug */
								if ( maze[*r + 1][*c] == 1 )
									*dir = DOWN ;
							}
						}
					}
					else
					{
						/* if thepàOÇOÄÄOåÄOÅÄOÅÉàOÅàÅœÄœÅÉàOåÄOåà àOå  Ñ	àà	¡œÄ  àOÅÉàOåÄOÅàÅœ¿¿ppÚrrrrr   àÄàÅœ√œÑ¡œåAœåœ¬O¿ppÚrrrrrÅéO¿O∆O¿ppÚrrrr ppÚrrr ppÚrrrÑppÚrrr ppÚrrrr@@œÑ åOÅÉààOÇOÄÄOáéÉœÇ œÅÉàOëò°œâÇàÇ  ¡œÅÑÇOé ÄÉàœáéÉœ¿¿
					*dir = random ( 4 ) ;
				}

				break ;

			caseä®ÖÖÑÑÑ¥≥6∞Ω2≠ï9êÆ≠ï1ÆêûêêÜÖÑÑÑΩÜÖÑÑÑÑπ2∞≤1¥0π9êñ1ñ:2∂∏êÜÖÖÑÑÑÑ¥≥:2∂∏ûêÜÖÑÑÑÑΩÜÖÑÑÑÑÑ¥≥6∞Ω2≠ï9.≠ï1êêÆêûêêÜÖÑÑÑÑÑÑï24πê)$£§*ÜÖÑÑÑÑÑ≤∂9≤ÜÖÑÑÑÑÑΩÜÖÑÑÑÑÑÑ¥≥ 6∞Ω2≠ï9.≠ï1êêÆêûêêÜÖÑÑÑÑÑÑÑï24πê&"£*ÜÖÑÑÑÑÑÑ≤∂9≤ÜÖÑÑÑÑÑÑΩÜÖÑÑÑÑÑÑÑ¥≥6∞Ω2≠ï9êÆ≠ï1ÆêûêêÜÖÑÑÑÑÑÑÑÑï24πê"'´ßÜÖÑÑÑÑÑÑæÜÖÑÑÑÄÑæÜÖÑÑÑÑæÜÖÑÑÑÑ≤∂9≤ÜÖÑÑÑÑΩÜÖÑÑÑÑÑ∂∑ª2±:≥∫∏91ñ1¥êÜÖÑÑÑÑÑ≥60≥êêêÜÖÑÑÑÑæÜÖ				}
				else
					*dir = random ( 4 ) ;

				break ;

			case DOWN :

				if ( maze[*r + 1][*c] == 1 )
				{
					readchar ( *r + 1, *c, &temp ) ;

					if ( temp == 2 )
					{
						if ( maze[*r][*c + 1] == 1 )
							*dir = RIGHT ;
						else
						{
							if ( maze[*r][*c - 1] == 1 )
								*dir = LEFT ;
							else
							{
								if ( maze[*r - 1][*c] == 1 )
									*dir = UP ;
							}
						}
					}
					else
					{
						movebugdown ( r, c, ch ) ;
						flag = 1 ;
					}
				}
				else
					*dir = random ( 4 ) ;

				break ;
		}

		/* if the bug has been moved, take control outside the loop */
		if ( flag == 1 )
			break ;

		trials++ ;

		/* if even after 15 trials the correct direction cannot be found, then give up */
		if ( trials > 15 )
			break ;
	}

	/* if the bug collides with the Eater, kill it */
	if ( *r == row && *c == col )
		killeater() ;
}

/* moves the bug to the left of its current position */
movebugleft ( int *row, int *colm, unsigned char *ch )
{
	/* place back the character originally at the position occupied by the bug */
	writechar ( *ch, *row, *colm, 63 ) ;

	/* update column */
	*colm = *colm - 1 ;

	/* find out the character over which the bug is to be placed */
	readchar ( *row, *colm, ch ) ;

	/* if Eater is present at that position */
	if ( *ch == 1 )
		*ch = ' ' ;  /* associate with the bug a space as the character under it */

	/* place the bug at the updated position */
	writechar ( 2, *row, *colm, 112 ) ;
}

/* moves the bug to the right of its current position */
movebugright ( int *row, int *colm, unsigned char *ch )
{
	writechar ( *ch, *row, *colm, 63 ) ;
	*colm = *colm + 1 ;
	readchar ( *row, *colm, ch ) ;

	if ( *ch == 1 )
		*ch = ' ' ;

	writechar ( 2, *row, *colm, 112 ) ;
}

/* moves the bug above its current position */
movebugup ( int *row, int *col, unsigned char *ch )
{
	writechar ( *ch, *row, *col, 63 ) ;
	*row = *row - 1 ;
	readchar ( *row, *col, ch ) ;

	if ( *ch == 1 )
		*ch = ' ' ;

	writechar ( 2, *row, *col, 112 ) ;
}

/* moves the bug below its current position */
movebugdown ( int *row, int *col, unsigned char *ch )
{
	writechar ( *ch, *row, *col, 63 ) ;
	*row = *row + 1 ;
	readchar ( *row, *col, ch ) ;

	if ( *ch == 1 )
		*ch = ' ' ;

	writechar ( 2, *row, *col, 112 ) ;
}

/* issues interrupt to change the size of the cursor */
size ( int ssl, int esl )
{
	union REGS i, o ;

	i.h.ah = 1 ;  /* service number */
	i.h.ch = ssl ;  /* starting scan line */
	i.h.cl = esl ;  /* ending scan line */
	i.h.bh = 0 ;  /* video page */

	int86 ( 16, &i, &o ) ;  /* issue interrupt */
}

/* plays different types of music */
music ( int type )
{
	/* natural frequencies of 7 notes */
	float octave[7] = { 130.81, 146.83, 164.81, 174.61, 196, 220, 246.94 } ;
	int n, i ;

	switch ( type )
	{
		case 1 :

			/* continue playing music till a key is hit */
			while ( !kbhit() )
			{
				n = random ( 7 ) ;
				sound ( octave[n] * 4 ) ;
				delay ( 100 ) ;
			}
			nosound() ;

			/* flush the keyboard buffer */
			if ( getch() == 0 )
				getch() ;

			break ;

		case 2 :
			for ( i = 6 ; i >= 0 ; i-- )
			{
				sound ( octave[i] ) ;
				delay ( 54 ) ;
			}
			nosound() ;
			break ;

		case 3 :
			sound ( octave[6] * 2 ) ;
			delay ( 50 ) ;
			nosound() ;
	}
}

/* kills the Eater */
killeater()
{
	int r, c ;

	/* save the values of row and column */
	r = row ;
	c = col ;

	/* write the Eater */
	writechar ( 1, row, col, 112 ) ;

	/* show the dead Eater below the maze, signifying the life lost */
	writechar ( 1, 23, 15 + liveslost * 3, 112 ) ;

	music ( 2 ) ;

	/* one more life of Eater lost */
	liveslost++ ;

	/* if all three lives of the Eater are lost */
	if ( liveslost == 3 )
		return ;

	/* place the new Eater at the center of the maze */
	row = 11 ;
	col = 40 ;
	writechar ( 1, row, col, 112 ) ;

	/* place the bug which ate the Eater back in its position */
	writechar ( 2, r, c, 112 ) ;
}

/* creates ending screen */
endscreen()
{
	char ans = 0 ;

	/* initialise the graphics system */
	initgraph ( &gd, &gm, "c:\\tc\\bgi" ) ;

	/* draw a double-lined box */
	rectangle ( 0, 0, maxx, maxy ) ;
	rectangle ( 2, 2, maxx - 2, maxy - 2 ) ;

	/* set the font and alignment of the text to be displayed */
	settextjustify ( CENTER_TEXT, CENTER_TEXT ) ;
	settextstyle ( TRIPLEX_FONT, HORIZ_DIR, 3 ) ;

	/* if all three lives of the Eater are lost */
	if ( liveslost == 3 )
	{
		outtextxy ( midx, midy - 30, "Bad luck !!" ) ;
		outtextxy ( midx, midy, "Try again" ) ;
	}
	else
	{
		/* if all titbits have been eaten up */
		outtextxy ( midx, midy / 2, "You really are a" ) ;
		settextstyle ( DEFAULT_FONT, HORIZ_DIR, 6 ) ;
		outtextxy ( midx, midy, "GENIUS!!" ) ;
	}

	settextstyle ( TRIPLEX_FONT, HORIZ_DIR, 3 ) ;
	outtextxy ( midx, midy + midy / 2, "Another game (Y/N)..." ) ;

	/* continue till a correct answer is supplied */
	while ( ! ( ans == 'Y' || ans == 'N' ) )
	{
		fflush ( stdin ) ;
		ans = getch() ;
		ans = toupper ( ans ) ;
	}

	/* change over to text mode */
	closegraph() ;
	restorecrtmode() ;

	/* send back choice made */
	return ( ans ) ;
}

/* initialises variables at the start of each game */
initialise()
{
	int j ;

	/* initialise row and column of Eater */
	row = 11 ;
	col = 40 ;

	/* initialise starting row and column of bugs */
	r[0] = 3 ;	c[0] = 76 ;
	r[1] = 3 ;	c[1] = 22 ;
	r[2] = 19 ; 	c[2] = 46 ;
	r[3] = 11 ; 	c[3] = 5 ;
	r[4] = 19 ; 	c[4] = 63 ;
	score = 0 ;
	liveslost = 0 ;

	bugnumber = 0 ;

	/* initialise direction and the character under each bug */
	for ( j = 0 ; j < 5 ; j++ )
	{
		dir[j] = 0 ;
		charbelow[j] = 250 ;
	}
}
