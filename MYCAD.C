/* Refer Appendix B in the book if you want to create standalone
   EXE file of this program during compilation */

# include "graphics.h"
# include "math.h"
# include "dos.h"
# include "process.h"
# include "alloc.h"
# include "stdlib.h"
# include "ctype.h"
# include "stdio.h"
# include "fcntl.h"
# include "types.h"
# include "stat.h"

/* macro definitions for various keys and their scan/ascii codes */
# define ESC			27
# define ENTER			13
# define DOWN			80
# define LEFT			75
# define RIGHT			77
# define UP				72
# define HOME			71
# define END			79
# define PGUP			73
# define PGDN			81
# define TAB			9
# define SH_TAB			15
# define CTRL_RIGHT		116
# define CTRL_LEFT		115
# define CTRL_PGDN		118
# define CTRL_PGUP		132
# define CTRL_HOME		119
# define CTRL_END		117

/* various menu definitions */
char *mainmenu[ ] = {
						"Draw",
						"Edit",
						"File",
						"Line style",
						"eXit"
				    } ;

char *drawmenu[ ] = {
						"Line",
						"Box",
						"Circle",
						"Ellipse",
						"Arc",
						"Freehand",
						"Text"
					} ;

char *editmenu[ ] = {
					"Wipe",
					"Erase",
					"Move",
					"Copy",
					"Paint"
				  } ;

char *filemenu[ ] = {
					"Save",
					"Load",
					"Print"
				 } ;

void *p, *q, *r ;
int x = 10, y = 10, ascii, scan ;

main()
{
	int choice, gm = CGAHI, gd = CGA ;
	size_t area, cursorarea ;

	/* initialise the graphics system */
	initgraph ( &gd, &gm, "c:\\tc\\bgi" ) ;

	/* allocate memory in RAM for saving screen contents */
	area = imagesize ( 0, 0, 637, 177 ) ;
	p = malloc ( area ) ;

	/* allocate memory in RAM for saving screen contents while drawing lines, boxes, circles etc. */
	r = malloc ( area ) ;

	/* allocate memory in RAM for saving area of screen on which the cursor is to be placed */
	cursorarea = imagesize ( 0, 0, 7, 7 ) ;
	q = malloc ( cursorarea ) ;

	/* if memory allocation fails, terminate execution */
	if ( p == NULL || q == NULL || r == NULL )
	{
		printf ( "Insufficient memory! Press any key..." ) ;
		fflush ( stdin ) ;
		getch() ;

		closegraph() ;  /* shut down graphics system */
		restorecrtmode() ;  /* restore original video mode */

		exit ( 1 ) ;
	}

	/* create opening screen display */
	mainscreen() ;

	/* clear the screen contents */
	clearviewport() ;

	/* define the drawing area */
	rectangle ( 0, 10, 639, 189 ) ;
	setviewport ( 1, 11, 638, 188, 1 ) ;

	/* display current cursor position */
	gotoxy ( 68, 25 ) ;
	printf ( "X:%3d  Y:%3d", x, y ) ;

	while ( 1 )
	{
		/* display main menu in topmost row */
		displaymenu ( mainmenu, 5 ) ;

		/* receive user's response */
		choice = getresponse ( "DEFLX", 5 ) ;

		/* call appropriate function */
		switch ( choice )
		{
			case 1 :
				draw() ;
				break ;

			case 2 :
				edit() ;
				break ;

			case 3 :
				file() ;
				break ;

			case 4 :
				linetype() ;
				break ;

			case 5 :
				closegraph() ;  /* shut down graphics system */
				restorecrtmode() ;  /* restore original video mode */
				exit ( 0 ) ;
		}
	}
}

/* creates the opening screen display */
mainscreen()
{
	int maxx, maxy, in, area ;

	/* get maximum x, y coordinates of the screen */
	maxx = getmaxx() ;
	maxy = getmaxy() ;

	/* draw rectangles */
	setcolor ( WHITE ) ;
	rectangle ( 0, 0, maxx, maxy ) ;
	rectangle ( 0, 10, 639, 189 ) ;

	/* open the file */
	in = open ( "screen.dwg", O_BINARY | O_RDONLY ) ;
	if ( in == -1 )
		return ;

	/* find area required to accomodate file contents in memory */
	area = imagesize ( 0, 0, 637, 177 ) ;

	/* read the file contents into allocated memory */
	read ( in, p, area ) ;

	/* close the file */
	close ( in ) ;

	/* display the contents of allocated memory on the screen */
	putimage ( 1, 11, p, COPY_PUT ) ;

	/* set line style and text justification */
	settextstyle ( DEFAULT_FONT, HORIZ_DIR, 0 ) ;
	settextjustify ( CENTER_TEXT, TOP_TEXT ) ;

	/* output text */
	outtextxy ( maxx / 2, 2, "MYCAD" ) ;
	outtextxy ( 39 * 8, 24 * 8 - 1, "Press any key to continue..." ) ;

	fflush ( stdin ) ;
	getch() ;

	/* clear the viewport area */
	clearviewport() ;
}

/* displays Draw menu, receives choice and branches control to appropriate function */
draw()
{
	int dchoice ;

	while ( 1 )
	{
		displaymenu ( drawmenu, 7 ) ;

		gotoxy ( 1, 25 ) ;
		printf ( "Press Esc to return to previous menu" ) ;

		dchoice = getresponse ( "LBCEAFT", 7 ) ;

		switch ( dchoice )
		{
			case 1 :
				drawline() ;
				break ;

			case 2 :
				drawbox() ;
				break ;

			case 3 :
				drawcircle() ;
				break ;

			case 4 :
				drawellipse() ;
				break ;

			case 5 :
				drawarc() ;
				break ;

			case 6 :
				freehand() ;
				break ;

			case 7 :
				text() ;
				break ;

			case ESC :
				return ;
		}
	}
}

/* displays Edit menu, receives choice and branches control to appropriate function */
edit()
{
	int echoice ;

	while ( 1 )
	{
		displaymenu ( editmenu, 5 ) ;

		gotoxy ( 1, 25 ) ;
		printf ( "Press Esc to return to previous menu" ) ;

		echoice = getresponse ( "WEMCP", 5 ) ;

		switch ( echoice )
		{
			case 1 :
				wipe() ;
				break ;

			case 2 :
				erase() ;
				break ;

			case 3 :
				move() ;
				break ;

			case 4 :
				copy() ;
				break ;

			case 5 :
				paint() ;
				break ;

			case ESC :
				return ;
		}
	}
}

/* displays File menu, receives choice and branches control to appropriate function */
file()
{
	int fchoice ;

	while ( 1 )
	{
		displaymenu ( filemenu, 3 ) ;

		gotoxy ( 1, 25 ) ;
		printf ( "Press Esc to return to previous menu" ) ;

		fchoice = getresponse ( "SLP", 3 ) ;

		switch ( fchoice )
		{
			case 1 :
				save() ;
				break ;

			case 2 :
				load() ;
				break ;

			case 3 :
				print() ;
				break ;

			case ESC :
				return ;
		}
	}
}

/* displays the given menu in topmost row */
displaymenu ( char **menu, int count )
{
	int col = 2, i ;

	cleartext ( 2 ) ;  /* erase the top and bottom row */

	for ( i = 0 ; i < count ; i++ )
	{
		gotoxy ( col, 1 ) ;
		printf ( "%s", menu[i] ) ;
		col = col + 79 / count ;
	}
}

/* receives user's choice */
getresponse ( char *hotkeys, int count )
{
	int col, choice = 1, hotkeychoice, len ;

	while ( 1 )
	{
		/* calculate the column in which the triangle is to be placed */
		col = ( choice - 1 ) * ( 79 / count ) + 1 ;

		/* display triangle to indicate current menu item */
		gotoxy ( col, 1 ) ;
		putchar ( 16 ) ;

		/* receive a key from keyboard */
		getkey() ;

		/* if special key is hit */
		if ( ascii == 0 )
		{
			/* erase triangle */
			gotoxy ( col, 1 ) ;
			putchar ( ' ' ) ;

			/* test if left or right arrow key is hit */
			switch ( scan )
			{
				case RIGHT :
					choice++ ;
					break ;

				case LEFT :
					choice-- ;
			}

			/* if triangle is on last item and right arrow key is hit */
			if ( choice > count )
				choice = 1 ;

			/* if triangle is on first item and left arrow key is hit */
			if ( choice == 0 )
				choice = count ;
		}
		else
		{
			if ( ascii == ENTER )
				return ( choice ) ;

			if ( ascii == ESC )
				return ( ESC ) ;

			/* check whether a hot key has been pressed */

			len = strlen ( hotkeys ) ;
			hotkeychoice = 1 ;
			ascii = toupper ( ascii ) ;

			while ( *hotkeys )
			{
				if ( *hotkeys == ascii )
					return ( hotkeychoice ) ;
				else
				{
					hotkeys++ ;
					hotkeychoice++ ;
				}
			}

			/* reset hotkeys pointer */
			hotkeys = hotkeys - len ;

			/* beep to indicate invalid choice */
			printf ( "\a" ) ;
		}
	}
}

/* gets ascii and scan codes of the key pressed */
getkey()
{
	union REGS i, o ;

	/* wait till a key is pressed */
	while ( !kbhit() )
		;

	i.h.ah = 0 ;  /* service number */

	/* issue interrupt */
	int86 ( 22, &i, &o ) ;

	ascii = o.h.al ;
	scan = o.h.ah ;
}

/* clears top and/or bottom row on the screen */
cleartext ( int n )
{
	int i ;

	switch ( n )
	{
		case 0 :  /* erase top row */
			for ( i = 1 ; i <= 80 ; i++ )
			{
				gotoxy ( i, 1 ) ;
				putchar ( ' ' ) ;
			}
			break ;

		case 1 :  /* erase bottom row */
			for ( i = 1 ; i <= 67 ; i++ )
			{
				gotoxy ( i, 25 ) ;
				putchar ( ' ' ) ;
			}
			break ;

		case 2 :  /* erase top and bottom row */
			cleartext ( 0 ) ;
			cleartext ( 1 ) ;
			break ;
	}
}

drawline()
{
	int lchoice, x1, y1, x2, y2 ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "LINE" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	while ( 1 )
	{
		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Select the starting point" ) ;

		/* save current screen contents */
		getimage ( 0, 0, 637, 177, p ) ;

		/* allow movement of cursor and selection of starting point */
		lchoice = movecursor() ;

		if ( lchoice == ESC )
			return ;

		/* save starting point coordinates */
		x1 = x ;
		y1 = y ;

		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Use arrow keys to draw, Enter to confirm" ) ;

		/* allow interactive drawing of line */
		while ( 2 )
		{
			/* store the image of screen where line is to be drawn */
			getimage ( x1, y1, x, y, r ) ;

			/* draw line */
			moveto ( x1, y1 ) ;
			lineto ( x, y ) ;

			/* receive key */
			getkey() ;

			/* erase line */
			setcolor ( BLACK ) ;
			moveto ( x1, y1 ) ;
			lineto ( x, y ) ;
			setcolor ( WHITE ) ;

			/* assign to x2 the smaller of x1 and x, and to y2 that of y1 and y */
			x2 = x1 < x ? x1 : x ;
			y2 = y1 < y ? y1 : y ;

			/* restore the image on screen where line was drawn */
			putimage ( x2, y2, r, OR_PUT ) ;

			/* test the key that has been pressed */
			lchoice = testkeys() ;

			/* if Esc key is pressed, abandon line drawing */
			if ( lchoice == ESC )
				return ;

			/* if Enter key is pressed, make the line permanent */
			if ( lchoice == ENTER )
			{
				moveto ( x1, y1 ) ;
				lineto ( x, y ) ;

				/* line completed, hence start with a new line */
				break ;
			}
		}
	}
}

/* allow movement of cursor until Enter or Esc key is pressed */
movecursor()
{
	int choice ;

	while ( 1 )
	{
		/* draw the cursor at current x, y coordinates */
		drawcursor ( 1 ) ;

		/* receive key */
		getkey() ;

		/* erase the cursor from current x, y coordinates */
		drawcursor ( 0 ) ;

		/* test the key that has been hit */
		choice = testkeys() ;

		if ( choice == ESC || choice == ENTER )
			return ( choice ) ;
	}
}

/* draws or erases cursor as per the value of color */
drawcursor ( int color )
{
	int xx, yy ;

	/* save image if cursor is to be drawn */
	if ( color == 1 )
		getimage ( x - 3, y - 3, x + 3, y + 3, q ) ;

	/* draw horizontal line of the cursor */
	for ( xx = x - 3 ; xx <= x + 3 ; xx++ )
		putpixel ( xx, y, color ) ;

	/* draw vertical line of the cursor */
	for ( yy = y - 3 ; yy <= y + 3 ; yy++ )
		putpixel ( x, yy, color ) ;

	/* restore image if the cursor has been erased */
	if ( color == 0 )
		putimage ( x - 3, y - 3, q, OR_PUT ) ;
}

/* tests which key has been hit */
testkeys()
{
	/* if cursor movement key is hit, update values of x, y */
	if ( ascii == 0 )
	{
		switch ( scan )
		{
			case DOWN :
				y++ ;
				break ;

			case UP :
				y-- ;
				break ;

			case LEFT :
				x-- ;
				break ;

			case RIGHT :
				x++ ;
				break ;

			case HOME :
				y-- ;
				x-- ;
				break ;

			case END :
				x-- ;
				y++ ;
				break ;

			case PGUP :
				x++ ;
				y-- ;
				break ;

			case PGDN :
				x++ ;
				y++ ;
				break ;

			case SH_TAB :
				x -= 10 ;
				break ;

			case CTRL_RIGHT :
				x = 636 ;
				break ;

			case CTRL_LEFT :
				x = 2 ;
				break ;

			case CTRL_PGDN :
				y += 5 ;
				break ;

			case CTRL_PGUP :
				y -= 5 ;
				break ;

			case CTRL_HOME :
				x = 2 ;
				y = 2 ;
				break ;

			case CTRL_END :
				x = 636 ;
				y = 176 ;
		}
	}
	else
	{
		if ( ascii == ENTER )
			return ( ENTER ) ;

		if ( ascii == ESC )
		{
			putimage ( 0, 0, p, OR_PUT ) ;  /* restore screen contents */
			return ( ESC ) ;
		}

		if ( ascii == TAB )
			x += 10 ;
	}

	/* readjust x, y if their values exceed the limits */

	if ( x > 635 )
		x = 635 ;

	if ( x < 3 )
		x = 3 ;

	if ( y > 175 )
		y = 175 ;

	if ( y < 3 )
		y = 3 ;

	/* display new x, y coordinates */
	gotoxy ( 68, 25 ) ;
	printf ( "X:%3d  Y:%3d", x, y ) ;

	/* if a key other than Enter or Esc is pressed */
	return ( 100 ) ;
}

drawbox()
{
	int bchoice, x1, y1, x2, y2 ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "BOX" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	while ( 1 )
	{
		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Select the first corner" ) ;

		/* save current screen contents */
		getimage ( 0, 0, 637, 177, p ) ;

		/* allow movement of cursor and selection of starting corner */
		bchoice = movecursor() ;

		if ( bchoice == ESC )
			return ;

		/* save coordinates of starting corner */
		x1 = x ;
		y1 = y ;

		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Use arrow keys to select other corner, Enter to confirm" ) ;

		/* allow interactive drawing of box */
		while ( 2 )
		{
			/* store the image of screen where box is to be drawn */
			getimage ( x1, y1, x, y, r ) ;

			/* draw box */
			rectangle ( x1, y1, x, y ) ;

			/* receive key */
			getkey() ;

			/* erase box */
			setcolor ( BLACK ) ;
			rectangle ( x1, y1, x, y ) ;
			setcolor ( WHITE ) ;

			/* assign to x2 the smaller of x1 and x, and to y2 that of y1 and y */
			x2 = x1 < x ? x1 : x ;
			y2 = y1 < y ? y1 : y ;

			/* restore the image on screen where box was drawn */
			putimage ( x2, y2, r, OR_PUT ) ;

			/* test the key that has been pressed */
			bchoice = testkeys() ;

			/* if Esc key is pressed, abandon box drawing */
			if ( bchoice == ESC )
				return ;

			/* if Enter key is pressed, make the box permanent */
			if ( bchoice == ENTER )
			{
				rectangle ( x1, y1, x, y ) ;

				/* box completed, hence start with a new box */
				break ;
			}
		}
	}
}

drawcircle()
{
	int cchoice, radius, xc, yc, x1, x2, y1, y2 ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "CIRCLE" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	while ( 1 )
	{
		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Select the centre of circle" ) ;

		/* save current screen contents */
		getimage ( 0, 0, 637, 177, p ) ;

		/* allow movement of cursor and selection of the centre of circle */
		cchoice = movecursor() ;

		if ( cchoice == ESC )
			return ;

		/* save coordinates of centre of circle */
		xc = x ;
		yc = y ;

		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Use Left & Right arrow keys to draw circle, Enter to confirm" ) ;

		/* allow interactive drawing of circle */
		while ( 2 )
		{
			/* calculate radius of circleÁ 8B<œœ#‚`€ðãƒ!ðóàóðàãð“ð<<œœ3ã€c  c€bã  "`€#€bà3à`âá" c€" bà€3à àƒ óà`âã # bà®däL¤-ÎlnM,L¬„Eá¡A!!/$¤d¤L,.®da¡A!!/D¤ddL,.®da¡A!!/&$¤,d- 0.5 * radius ;
			y2 = yc + 0.5 * radius ;

			/* readjust x1, y1, x2, y2 if their values exceed the limits */

			if ( x1 < 2 )
				x1 = 2 ;

			if ( x2 > 636 )
				x2 = 636 ;

			if ( y1 < 2 )
				y1 = 2 ;

			if ( y2 > 176 )
				y2 = 176 ;

			/* store the image of screen where circle is to be drawn */
			getimage ( x1, y1, x2, y2, r ) ;

			/* draw circle */
			circle ( xc, yc, radius ) ;

			/* receive key */
			getkey() ;

			/* erase circle */
			setcolor ( BLACK ) ;
			circle ( xc, yc, radius ) ;
			setcolor ( WHITE ) ;

			/* restore the image on screen where circle was drawn */
			putimage ( x1, y1, r, OR_PUT ) ;

			/* test the key that has been pressed */
			cchoice = testkeys() ;

			/* if Esc key is pressed, abandon circle drawing */
			if ( cchoice == ESC )
				return ;

			/* if Enter key is pressed, make the circle permanent */
			if ( cchoice == ENTER )
			{
				circle ( xc, yc, radius ) ;

				/* circle completed, hence start with a new circle */
				break ;
			}
		}
	}
}

drawellipse()
{
	int echoice, xradius, yradius, x1, y1, x2, y2, xc, yc ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "ELLIPSE" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	while ( 1 )
	{
		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Select the centre of ellipse" ) ;

		/* save current screen contents */
		getimage ( 0, 0, 637, 177, p ) ;

		/* allow movement of cursor and selection of the centre of ellipse */
		echoice = movecursor() ;

		if ( echoice == ESC )
			return ;

		/* save coordinates of centre of ellipse */
		xc = x ;
		yc = y ;

		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Use arrow keys to draw ellipse, Enter to confirm" ) ;

		/* allow interactive drawing of ellipse */
		while ( 2 )
		{
			/* calculate major and minor axes of the ellipse */
			xradius = abs ( x - xc ) ;
			yradius = abs ( y - yc ) ;

			/* calculate coordinates of the rectangle in which the ellipse is to be inscribed */
			x1 = xc - xradius ;
			x2 = xc + xradius ;
			y1 = yc - yradius ;
			y2 = yc + yradius ;

			/* readjust x1, y1, x2, y2 if their values exceed the limits */

			if ( x1 < 2 )
				x1 = 2 ;

			if ( x2 > 636 )
				x2 = 636 ;

			if ( y1 < 2 )
				y1 = 2 ;

			if ( y2 > 176 )
				y2 = 176 ;

			/* store the image of screen where ellipse is to be drawn */
			getimage ( x1, y1, x2, y2, r ) ;

			/* draw ellipse */
			ellipse ( xc, yc, 0, 360, xradius, yradius ) ;

			/* receive key */
			getkey() ;

			/* erase ellipse */
			setcolor ( BLACK ) ;
			ellipse ( xc, yc, 0, 360, xradius, yradius ) ;
			setcolor ( WHITE ) ;

			/* restore the image on screen where ellipse was drawn */
			putimage ( x1, y1, r, OR_PUT ) ;

			/* test the key that has been pressed */
			echoice = testkeys() ;

			/* if Esc key is pressed, abandon ellipse drawing */
			if ( echoice == ESC )
				return ;

			/* if Enter key is pressed, make ellipse permanent */
			if ( echoice == ENTER )
			{
				ellipse ( xc, yc, 0, 360, xradius, yradius ) ;

				/* ellipse completed, hence start with new ellipse */
				break ;
			}
		}
	}
}

drawarc()
{
	int achoice, xc, yc, start, end, x1, y1, x2, y2 ;
	double st_angle, en_angle, radius, s, a, b ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "ARC" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	while ( 1 )
	{
		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Select the centre of arc" ) ;

		/* save current screen contents */
		getimage ( 0, 0, 637, 177, p ) ;

		/* allow movement of cursor and selection of the centre of arc */
		achoice = movecursor() ;

		if ( achoice == ESC )
			return ;

		/* save coordinates of centre of arc */
		xc = x ;
		yc = y ;

		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Select the starting point of arc, Enter to confirm" ) ;

		/* test the key that has been pressed */
		achoice = movecursor() ;

		/* if Esc key is pressed, abandon arc drawing */
		if ( achoice == ESC )
			return ;

		/* find absolute difference between the x-coordinates of the center and starting point of arc */
		a = abs ( x - xc ) ;

		/* find absolute difference between the y-coordinates of the center and starting point of arc */
		b = 2 * abs ( y - yc ) ;  /* 2 takes care of the aspect ratio */

		/* calculate radius of the arc */
		radius = sqrt ( a * a + b * b ) ;

		/* calculate the starting angle */
		st_angle = asin ( b / radius ) ;

		/* convert from radians to degrees */
		start = st_angle * 180 / 3.14 ;

		/* if the starting point of the arc lies in the second quadrant */
		if ( ( x <= xc ) && ( y <= yc ) )
			start = 180 - start ;  /* take the complement of calculated angle */
		else
		{
			/* if the starting point of the arc lies in the third quadrant */
			if ( ( x <= xc ) && ( y > yc ) )
				start = 180 + start ;
			else
			{
				/* if the starting point of the arc lies in the fourth quadrant */
				if ( ( x > xc ) && ( y > yc ) )
					start = 360 - start ;
			}
		}

		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Use arrow keys to draw arc, Enter to confirm" ) ;

		/* calculate coordinates of the rectangle in which the arc is to be inscribed */
		x1 = xc - radius ;
		x2 = xc + radius ;
		y1 = yc - 0.5 * radius ;
		y2 = yc + 0.5 * radius ;

		/* readjust x1, y1, x2, y2 if their values exceed the limits */

		if ( x1 < 2 )
			x1 = 2 ;

		if ( x2 > 636 )
			x2 = 636 ;

		if ( y1 < 2 )
			y1 = 2 ;

		if ( y2 > 176 )
			y2 = 176 ;

		/* allow interactive drawing of arc */
		while ( 2 )
		{
			/* readjust x and y in instances where attempt is made to stretch the arc beyond its radius */

			if ( x < xc - radius )
				x = xc - radius ;

			if ( y < yc - radius )
				y = yc - radius ;

			if ( x > xc + radius )
				x = xc + radius ;

			if ( y > yc + radius )
				 y = yc + radius ;

			/* calculate ending angle */
			a = abs ( x - xc ) ;
			b = 2 * abs ( y - yc ) ;
			s = sqrt ( a * a + b * b ) ;
			en_angle = asin ( b / s ) ;
			end = en_angle * 180 / 3.14 ;

			/* readjust ending angle as per the quadrant in which the ending point lies */
			if ( ( x <= xc ) && ( y <= yc ) )
				end = 180 - end ;
			else
			{
				if ( ( x <= xc ) && ( y > yc ) )
					end = 180 + end ;
				else
				{
					if ( ( x > xc ) && ( y > yc ) )
						end = 360 - end ;
				}
			}

			/* store the image of screen where arc is to be drawn */
			getimage ( x1, y1, x2, y2, r ) ;

			/* draw arc */
			arc ( xc, yc, start, end, radius ) ;

			/* receive key */
			getkey() ;

			/* erase arc */
			setcolor ( BLACK ) ;
			arc ( xc, yc, start, end, radius ) ;
			setcolor ( WHITE ) ;

			/* restore the image on screen where arc was drawn */
			putimage ( x1, y1, r, OR_PUT ) ;

			/* test the key that has been pressed */
			achoice = testkeys() ;

			/* if Esc key is pressed, abandon arc drawing */
			if ( achoice == ESC )
				return ;

			/* if Enter key is pressed, make arc permanent */
			if ( achoice == ENTER )
			{
				arc ( xc, yc, start, end, radius ) ;

				/* arc completed, hence start with a new arc */
				break ;
			}
		}
	}
}

/* draws freehand drawing */
freehand()
{
	int fchoice ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "FREEHAND" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	while ( 1 )
	{
		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Select the starting point" ) ;

		/* save current screen contents */
		getimage ( 0, 0, 637, 177, p ) ;

		/* allow movement of cursor and selection of the starting point */
		fchoice = movecursor() ;

		if ( fchoice == ESC )
			return ;

		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Use arrow keys to draw, Enter to end" ) ;

		/* allow interactive freehand drawing */
		while ( 2 )
		{
			/* draw a pixel */
			putpixel ( x, y, 1 ) ;

			/* receive key */
			getkey() ;

			/* test the key that has been pressed */
			fchoice = testkeys() ;

			/* if Esc key is pressed, abandon freehand drawing */
			if ( fchoice == ESC )
			{
				/* restore original screen contents, thereby erasing the freehand drawing */
				putimage ( 0, 0, p, COPY_PUT ) ;
				return ;
			}

			/* if Enter key is pressed, make the freehand drawing permanent */
			if ( fchoice == ENTER )
			{
				/* freehand completed, hence start with a new freehand drawing */
				break ;
			}
		}
	}
}

/* displays text at specified location */
text()
{
	char ch, str[51] ;
	int tchoice, j = 0 , k = 0 , l = 0 ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "TEXT" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	while ( 1 )
	{
		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Enter the text:" ) ;

		/* receive the text entered */
		tchoice = getstring ( str, 17 ) ;

		if ( tchoice == ESC )
			return ;

		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Select the position to place the text, Enter to confirm" ) ;

		/* save current screen contents */
		getimage ( 0, 0, 637, 177, p ) ;

		/* allow movement of cursor and selection of the starting point */
		tchoice = movecursor() ;

		if ( tchoice == ESC )
			return ;

		/* output the text in default style and justification */
		settextstyle ( DEFAULT_FONT, HORIZ_DIR, 0 ) ;
		settextjustify ( LEFT_TEXT, TOP_TEXT ) ;
		outtextxy ( x, y, str ) ;

		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Want to change the font (Y/N) ?" ) ;
		fflush ( stdin ) ;
		ch = getch() ;

		if ( ch == ESC )
		{
			putimage ( 0, 0, p, COPY_PUT ) ;
			return ;
		}

		/* allow interactive changing of the font of the text */
		if ( ch == 'y' || ch == 'Y' )
		{
			cleartext ( 1 ) ;
			gotoxy ( 1, 25 ) ;
			printf ( "Press any key to examine the fonts, Enter to confirm" ) ;

			while ( 1 )
			{
				fflush ( stdin ) ;
				ch = getch() ;

				if ( ch == ENTER )
					break ;

				/* if Esc key is pressed, restore screen contents and return to Draw menu */
				if ( ch == ESC )
				{
					putimage ( 0, 0, p, COPY_PUT ) ;
					return ;
				}

				/* counter for different font styles, maximum 5 */
				j++ ;
				if ( j > 4 )
					j = 0 ;

				/* set the text style as indicated by j */
				settextstyle ( j, HORIZ_DIR, 1 ) ;

				/* output the text in the chosen style */
				putimage ( 0, 0, p, COPY_PUT ) ;
				outtextxy ( x, y, str ) ;
			}
		}

		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Want to change the size of text (Y/N) ?" ) ;
		fflush ( stdin ) ;
		ch = getch() ;

		/* if Esc key is pressed, return to Draw menu */
		if ( ch == ESC )
		{
			putimage ( 0, 0, p, COPY_PUT ) ;
			return ;
		}

		/* allow interactive changing of the size of the text */
		if ( ch == 'y' || ch == 'Y' )
		{
			cleartext ( 1 ) ;
			gotoxy ( 1, 25 ) ;
			printf ( "Press any key to examine the sizes, Enter to confirm" ) ;

			while ( 1 )
			{
				fflush ( stdin ) ;
				ch = getch() ;

				if ( ch == ENTER )
					break ;

				/* if Esc key is pressed, return to Draw menu */
				if ( ch == ESC )
				{
					putimage ( 0, 0, p, COPY_PUT ) ;
					return ;
				}

				/* counter for different character sizes, maximum 10 */
				k++ ;
				if ( k > 9 )
					k = 0 ;

				/* set style as indicated by j and size as indicated by k */
				settextstyle ( j, HORIZ_DIR, k ) ;
				putimage ( 0, 0, p, COPY_PUT ) ;
				outtextxy ( x, y, str ) ;
			}
		}

		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Want to change the direction (Y/N) ?" ) ;
		fflush ( stdin ) ;
		ch = getch() ;

		/* if Esc key is pressed, restore screen contents and return to Draw menu */
		if ( ch == ESC )
		{
			putimage ( 0, 0, p, COPY_PUT ) ;
			return ;
		}

		/* allow interactive changing of the direction of the text */
		if ( ch == 'y' || ch == 'Y' )
		{
			cleartext ( 1 ) ;
			gotoxy ( 1, 25 ) ;
			printf ( "Press any key to observe the directions, Enter to confirm" ) ;

			while ( 1 )
			{
				fflush ( stdin ) ;
				ch = getch() ;

				if ( ch == ENTER )
					break ;

				/* if Esc key is pressed, restore screen contents and return to Draw menu */
				if ( ch == ESC )
				{
					putimage ( 0, 0, p, COPY_PUT ) ;
					return ;
				}

				/* counter for different text directions, maximum 2 */
				l++ ;
				if ( l > 1 )
					l = 0 ;

				/* set style as indicated by j, size as indicated by k and direction as indicated by l */
				settextstyle ( j, l, k ) ;
				putimage ( 0, 0, p, COPY_PUT ) ;
				outtextxy ( x, y, str ) ;
			}
		}
	}
}

/* gets a string from keyboard */
getstring ( char *str, int col )
{
	int i = 0 ;
	char ch ;

	/* receive text */
	while ( 1 )
	{
		/* receive key */
		fflush ( stdin ) ;

		/* if special key like arrow keys etc. are hit, ignore them */
		if ( ( ch = getch() ) == 0 )
		{
			getch() ;
			continue ;
		}

		/* if Enter key is hit, terminate string */
		if ( ch == ENTER )
		{
			str[i] = '\0' ;
			break ;
		}

		/* if Esc key is hit, return to Draw menu */
		if ( ch == ESC )
			return ( ESC ) ;

		/* if Backspace key is hit and some text has been entered */
		if ( ch == '\b' && i > 0 )
		{
			i-- ;

			/* erase the previously entered character */
			gotoxy ( col + i, 25 ) ;
			printf ( " " ) ;

			gotoxy ( col + i, 25 ) ;
			continue ;
		}

		/* ignore control characters */
		if ( ch < 32 )
			continue ;

		/* if not a control character, display it on the screen */
		gotoxy ( col + i, 25 ) ;
		printf ( "%c", ch ) ;

		str[i] = ch ;
		i++ ;

		/* accept a maximum of 50 characters */
		if ( i == 50 )
		{
			str[i] = '\0' ;
			break ;
		}
	}

	return ( 0 ) ;
}

/* wipes out the entire drawing */
wipe()
{
	char ch ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "WIPE" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;
	gotoxy ( 1, 25 ) ;
	printf ( "Are you sure (Y/N) ?" ) ;

	fflush ( stdin ) ;
	ch = getch() ;

	/* if Esc key is pressed, return to Edit menu */
	if ( ch == ESC )
		return ;

	/* clear the viewport */
	if ( ch == 'y' || ch == 'Y' )
		clearviewport() ;
}

/* erases the selected portion (here onwards called `object') on the screen */
erase()
{
	int echoice, x1, y1, x2, y2 ;
	struct linesettingstype linfo ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "ERASE" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;
	gotoxy ( 1, 25 ) ;
	printf ( "Select the first corner" ) ;

	/* save current screen contents */
	getimage ( 0, 0, 637, 177, p ) ;

	/* allow movement of cursor and selection of starting corner */
	echoice = movecursor() ;

	if ( echoice == ESC )
		return ;

	/* save coordinates of the starting corner */
	x1 = x ;
	y1 = y ;

	/* get the current line settings */
	getlinesettings ( &linfo ) ;

	/* set the line settings to the default values */
	setlinestyle ( SOLID_LINE, 0, NORM_WIDTH ) ;

	cleartext ( 1 ) ;
	gotoxy ( 1, 25 ) ;
	printf ( "Use arrow keys to draw rectangle, Enter to confirm" ) ;

	/* allow interactive drawing of box */
	while ( 2 )
	{
		/* store the image of screen where box is to be drawn */
		getimage ( x1, y1, x, y, r ) ;

		/* draw box */
		rectangle ( x1, y1, x, y ) ;

		/* receive key */
		getkey() ;

		/* erase box */
		setcolor ( BLACK ) ;
		rectangle ( x1, y1, x, y ) ;
		setcolor ( WHITE ) ;

		/* assign to x2 the smaller of x1 and x, and to y2 that of y1 and y */
		x2 = x1 < x ? x1 : x ;
		y2 = y1 < y ? y1 : y ;

		/* restore the image on screen where box was drawn */
		putimage ( x2, y2, r, OR_PUT ) ;

		/* test the key that has been pressed */
		echoice = testkeys() ;

		/* if Esc key is pressed, abandon erasing operation */
		if ( echoice == ESC )
			break ;

		/* if Enter key is pressed, erase object */
		if ( echoice == ENTER )
		{
			putimage ( x2, y2, r, XOR_PUT ) ;
			break ;
		}
	}

	/* restore the original line settings */
	setlinestyle ( linfo.linestyle, linfo.upattern, linfo.thickness ) ;
}

/* moves an `object' on screen to a new place */
move()
{
	int mchoice, x1, x2, y1, y2 ;
	struct linesettingstype linfo ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "MOVE" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;
	gotoxy ( 1, 25 ) ;
	printf ( "Select the first corner" ) ;

	/* save current screen contents */
	getimage ( 0, 0, 637, 177, p ) ;

	/* allow movement of cursor and selection of starting corner */
	mchoice = movecursor() ;

	if ( mchoice == ESC )
		return ;

	/* save coordinates of starting corner */
	x1 = x ;
	y1 = y ;

	/* get the current line settings */
	getlinesettings ( &linfo ) ;

	/* set the line settings to the default values */
	setlinestyle ( SOLID_LINE, 0, NORM_WIDTH ) ;

	cleartext ( 1 ) ;
	gotoxy ( 1, 25 ) ;
	printf ( "Use arrow keys to draw rectangle, Enter to confirm" ) ;

	/* allow interactive drawing of box */
	while ÏÄÏÔ†…½†…„—•9º7¹2:424¶°³²7³9±¹2²·;´2¹217¼4¹:712290»·†…„³²º4¶°³²<–<˜–<<–9†……„—•290»17¼†…„¹2±º0·3¶2<–<˜–<<†……„—•92±²´»25²¼†…„³²º5²¼”†……„—•2¹0¹²€17¼†…„¹²º1·¶7¹!& ¡¥†…„¹2±º0·3¶2<–<˜–<<†…„¹²ºcolor ( WHITE ) ;

		/* assign to x2 the smaller of x1 and x, and to y2 that of y1 and y */
		x2 = x1 < x ? x1 : x ;
		y2 = y1 < y ? y1 : y ;

		/* restore the image on screen where box was drawn */
		putimage ( x2, y2, r, OR_PUT ) ;

		/* test the key that has been pressed */
		mchoice = testkeys() ;

		/* if Esc key is pressed, abandon the move */
		if ( mchoice == ESC )
		{
			/* restore the original line settings */
			setlinestyle ( linfo.linestyle, linfo.upattern, linfo.thickness ) ;
			return ;
		}

		/* if Enter key is pressed */
		if ( mchoice == ENTER )
		{
			/* assign to x1 the greater of x1 and x, and to y1 that of y1 and y */
			x1 = x1 > x ? x1 : x ;
			y1 = y1 > y ? y1 : y ;

			/* save coordinates of starting position of object to be moved */
			x = x2 ;
			y = y2 ;

			cleartext ( 1 ) ;
			gotoxy ( 1, 25 ) ;
			printf ( "Use arrow keys to move to final position, Enter to confirm" ) ;

			/* allow interactive movement of the object */
			while ( 3 )
			{
				/* receive key */
				getkey() ;

				/* erase object */
				putimage ( x, y, r, XOR_PUT ) ;

				/* test the key that has been pressed */
				mchoice = testkeys() ;

				/* if the object being moved exceeds limits, readjust its position */

				if ( x > 636 - abs ( x1 - x2 ) )
					x = 636 - abs ( x1 - x2 ) ;

				if ( y > 176 - abs ( y1 - y2 ) )
					y = 176 - abs ( y1 - y2 ) ;

				/* if Esc key is pressed, abandon the movement */
				if ( mchoice == ESC )
				{
					/* restore the original line settings */
					setlinestyle ( linfo.linestyle, linfo.upattern,
								linfo.thickness ) ;

					return ;
				}

				/* if Enter key is pressed, make the object permanent */
				if ( mchoice == ENTER )
				{
					/* restore the original line settings */
					setlinestyle ( linfo.linestyle, linfo.upattern, linfo.thickness ) ;

					putimage ( x, y, r, XOR_PUT ) ;
					return ;
				}

				/* draw object */
				putimage ( x, y, r, XOR_PUT ) ;
			}
		}
	}
}

/* copies an `object' on screen to a new place */
copy()
{
	int cchoice, x1, x2, y1, y2, flag = 1 ;
	struct linesettingstype linfo ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "COPY" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	gotoxy ( 1, 25 ) ;
	printf ( "Select the first corner" ) ;

	/* save current screen contents */
	getimage ( 0, 0, 637, 177, p ) ;

	/* allow movement of cursor and selection of starting corner */
	cchoice = movecursor() ;

	if ( cchoice == ESC )
		return ;

	/* save coordinates of starting corner */
	x1 = x ;
	y1 = y ;

	/* get the current line settings */
	getlinesettings ( &linfo ) ;

	/* set the line settings to the default values */
	setlinestyle ( SOLID_LINE, 0, NORM_WIDTH ) ;

	cleartext ( 1 ) ;
	gotoxy ( 1, 25 ) ;
	printf ( "Use arrow keys to draw rectangle, Enter to confirm" ) ;

	/* allow interactive drawing of box */
	while ( 1 )
	{
		/* store the image of screen where box is to be drawn */
		getimage ( x1, y1, x, y, r ) ;

		/* draw box */
		rectangle ( x1, y1, x, y ) ;

		/* receive key */
		getkey() ;

		/* erase box */
		setcolor ( BLACK ) ;
		rectangle ( x1, y1, x, y ) ;
		setcolor ( WHITE ) ;

		/* test the key that has been pressed */
		cchoice = testkeys() ;

		/* assign to x2 the smaller of x1 and x, and to y2 that of y1 and y */
		x2 = x1 < x ? x1 : x ;
		y2 = y1 < y ? y1 : y ;

		/* restore the image on screen where box was drawn */
		putimage ( x2, y2, r, OR_PUT ) ;

		/* if Esc key is pressed, abandon copying operation */
		if ( cchoice == ESC )
		{
			/* restore the original line settings */
			setlinestyle ( linfo.linestyle, linfo.upattern, linfo.thickness ) ;

			return ;
		}

		/* if Enter key is pressed */
		if ( cchoice == ENTER )
		{
			/* assign to x1 the greater of x1 and x, and to y1 that of y1 and y */
			x1 = x1 > x ? x1 : x ;
			y1 = y1 > y ? y1 : y ;

			/* save coordinates of starting position of the object to be copied */
			x = x2 ;
			y = y2 ;

			cleartext ( 1 ) ;
			gotoxy ( 1, 25 ) ;
			printf ( "Use arrow keys to move, Enter to confirm" ) ;

			/* allow interactive copying of the object */
			while ( 3 )
			{
				/* receive key */
				getkey() ;

				/* erase the object except when in original position */
				if ( flag != 1 )
					putimage ( x, y, r, XOR_PUT ) ;

				flag = 0 ;

				/* test the key that has been pressed */
				cchoice = testkeys() ;

				/* if the object being moved exceeds limits, readjust its position */

				if ( x > 636 - abs ( x1 - x2 ) )
					x = 636 - abs ( x1 - x2 ) ;

				if ( y > 176 - abs ( y1 - y2 ) )
					y = 176 - abs ( y1 - y2 ) ;

				/* if Esc key is pressed, abandon the copying operation */
				if ( cchoice == ESC )
				{
					/* restore the original line settings */
					setlinestyle ( linfo.linestyle, linfo.upattern, linfo.thickness ) ;

					return ;
				}

				/* if Enter key is pressed, make the copy permanent */
				if ( cchoice == ENTER )
				{
					/* restore the original line settings */
					setlinestyle ( linfo.linestyle, linfo.upattern, linfo.thickness ) ;

					putimage ( x, y, r, OR_PUT ) ;
					return ;
				}

				/* draw the image */
				putimage ( x, y, r, XOR_PUT ) ;
			}
		}
	}
}

/* fills a bounded area */
paint()
{
	int pchoice, i = 1 ;
	char ch ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "PAINT" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	gotoxy ( 1, 25 ) ;
	printf ( "Place cursor within the boundary to be painted, Enter to confirm" ) ;

	/* save current screen contents */
	getimage ( 0, 0, 637, 177, p ) ;

	/* allow movement of cursor and selection of the area to be painted */
	pchoice = movecursor() ;

	/* if Esc key is pressed, abandon the painting operation */
	if ( pchoice == ESC )
		return ;

	/* if Enter key is pressed, fill the selected area */
	if ( pchoice == ENTER )
	{
		setfillstyle ( SOLID_FILL, WHITE ) ;
		floodfill ( x, y, WHITE ) ;
	}

	cleartext ( 1 ) ;
	gotoxy ( 1, 25 ) ;
	printf ( "Want to change the fill pattern (Y/N) ?" ) ;

	fflush ( stdin ) ;
	ch = getch() ;

	if ( ch == ESC )
	{
		putimage ( 0, 0, p, COPY_PUT ) ;
		return ;
	}

	/* allow interactive changing of the fill style */
	if ( ch == 'y' || ch == 'Y' )
	{
		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Press any key to examine the fill pattern, Enter to confirm" ) ;

		while ( 1 )
		{
			fflush ( stdin ) ;
			ch = getch() ;

			if ( ch == ENTER )
				break ;

			/* counter for different fill styles, maximum 12 */
			i++ ;
			if ( i > 11 )
				i = 0 ;

			/* if Esc key is pressed, abandon the painting operation */
			if ( ch == ESC )
			{
				putimage ( 0, 0, p, COPY_PUT ) ;
				return ;
			}

			/* set fill style as indicated by i */
			setfillstyle ( i, WHITE ) ;

			/* restore original image and fill bounded region */
			putimage ( 0, 0, p, COPY_PUT ) ;
			floodfill ( x, y, WHITE ) ;
		}
	}
}

/* saves the current drawing into a file */
save()
{
	char fname[30] ;
	int out, area, retvalue ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "SAVE" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	gotoxy ( 1, 25 ) ;
	printf ( "Enter the file name:" ) ;

	/* get the name of the file */
	retvalue = getstring ( fname, 22 ) ;
	if ( retvalue == ESC )
		return ;

	/* open the file */
	out = open ( fname, O_BINARY | O_CREAT | O_RDWR, S_IWRITE ) ;

	/* if unable to open file */
	if ( out == -1 )
	{
		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Unable to open the file! Press any key..." ) ;
		fflush ( stdin ) ;
		getch() ;

		return ;
	}

	/* store the screen contents into allocated memory */
	getimage ( 0, 0, 637, 177, p ) ;

	/* find area occupied by screen image in memory */
	area = imagesize ( 0, 0, 637, 177 ) ;

	/* copy the stored contents into the file */
	write ( out, p, area ) ;

	/* close the file */
	close ( out ) ;
}

/* loads a drawing from file into memory */
load()
{
	char fname[30] ;
	int area, in, retvalue ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "LOAD" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	gotoxy ( 1, 25 ) ;
	printf ( "Enter the file name:" ) ;

	/* get the name of the file */
	retvalue = getstring ( fname, 22 ) ;
	if ( retvalue == ESC )
		return ;

	/* open the file */
	in = open ( fname, O_BINARY | O_RDONLY ) ;

	/* if unable to open file */
	if ( in == -1 )
	{
		cleartext ( 1 ) ;
		gotoxy ( 1, 25 ) ;
		printf ( "Unable to open the file! Press any key..." ) ;
		fflush ( stdin ) ;
		getch() ;

		return ;
	}

	/* find area required to accomodate file contents in memory */
	area = imagesize ( 0, 0, 637, 177 ) ;

	/* read the file contents into allocated memory */
	read ( in, p, area ) ;

	/* close the file */
	close ( in ) ;

	/* display the contents of allocated memory on the screen */
	putimage ( 0, 0, p, COPY_PUT ) ;
}

/* prints the current drawing on the printer */
print()
{
	char ch ;
	union REGS i, o ;

	cleartext ( 2 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "PRINT" ) ;
	gotoxy ( 69, 1 ) ;
	printf ( "Esc to exit" ) ;

	gotoxy ( 1, 25 ) ;
	printf ( "Set up the printer and press any key" ) ;

	fflush ( stdin ) ;
	ch = getch() ;

	/* if Esc key is pressed return to File menu */
	if ( ch == ESC )
		return ;

	/* issue interrupt for printing the graphic screen */
	int86 ( 5, &i, &o ) ;
}

/* displays Line style menu, receives choice & sets up appropriate line style */
linetype()
{
	int typechoice ;
	struct linesettingstype linfo ;

	/* get the current line settings */
	getlinesettings ( &linfo ) ;

	cleartext ( 2 ) ;

	/* display Line style menu in topmost row */
	displaylinemenu() ;

	gotoxy ( 1, 25 ) ;
	printf ( "Press Esc to return to previous menu" ) ;

	/* receive user's response */
	typechoice = getresponse ( "", 4 ) ;

	/* if Esc key is pressed return to Draw menu */
	if ( typechoice == ESC )
	{
		/* restore the original line settings */
		setlinestyle ( linfo.linestyle, linfo.upattern, linfo.thickness ) ;

		return ;
	}

	/* set line style as indicated by variable typechoice */
	setlinestyle ( typechoice - 1, 0, NORM_WIDTH ) ;
}

/* displays Line style menu */
displaylinemenu()
{
	int incr, j, col = 11 ;

	setviewport ( 0, 0, 639, 199, 1 ) ;
	incr = 540 / 4 ;

	/* display available line styles */
	for ( j = 0 ; j < 4 ; j++ )
	{
		setlinestyle ( j, 0, NORM_WIDTH ) ;
		moveto ( col, 4 ) ;
		lineto ( col + incr, 4 ) ;
		col = col + incr + 17 ;
	}

	/* set viewport back to normal */
	setviewport ( 1, 11, 638, 188, 1 ) ;
}
