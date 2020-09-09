/* Refer Appendix B in the book if you want to create standalone
   EXE file of this program during compilation */

/* =============== Checklist of important variables ================ */
/*
pointers:

parachute			points to image of parachute in memory
heli1       		points to image of 1st helicopter in memory
heli2       		points to image of 2nd helicopter in memory
gunbase     		points to image of gunbase in memory
para        		points to image of paratrooper in memory
blast       		points to image of helicopter blast in memory
clear       		points to blank image in memory
blast_man   		points to image of paratrooper blast in memory
clear_man   		points to blank image in memory

arrays:

heli_x[8]   		stores x coordinates of helicopters
heli_level[8]   	stores level ( y coordinates ) of helicopters
no_para[8]      	stores no. of paratroopers in each helicopter
h_reached[8]    	flags to determine whether heli. reached its destination or not
jump_pt[8][4]   	stores x coordinates of each paratrooper at point of jumping					from helicopter; coordinates change as paratroopers descend.
para_y[8][4]		stores y coordinates of each paratrooper in each helicopter
para_alive[8][4]    flags to determine whether paratroopers are alive or not
para_in_heli[8][4]  flags to determine whether paratroopers have jumped or not
p_reached[8][4]		flags to determine whether paratroopers reached the ground or not
bullet_present[50]	flags to determine whether bullets are available for firing or not
x_left[32]			stores x coordinates of paratroopers landed to the left of the gun
x_right[32]			stores x coordinates of paratroopers landed to the right of the gun
bullet_xdir[50]		stores x-direction of bullets ( x = a * cos ( angle ) )
bullet_ydir[50]     stores y-direction of bullets ( y = a * sin ( angle ) )
bullet_x[50]        stores current x coordinates of bullets
bullet_y[50]        stores current y coordinates of bullets

integers:

maxx                maximum x coordinate of the screen
maxy                maximum y coordinate of the screen
score               points earned by the player
finish              flag to determine the end of the game
lmove               flag to control the movement of the gun towards left
rmove               flag to control the movement of the gun towards right
lnum                total paratroopers reached to the left of the gun
rnum                total paratroopers reached to the right of the gun
la                  flag to start gun attack from left
ra                  flag to start attack from right
id_flag             increment/decrement flag for the angle of the gun
angle               angle of the gun
*/

/* ======================================================== */

# include "graphics.h"
# include "conio.h"
# include "stdio.h"
# include "process.h"
# include "alloc.h"
# include "stdlib.h"
# include "math.h"
# include "dos.h"

void *parachute, *heli1, *heli2, *gunbase, *para ;
void *blast, *clear, *blast_man, *clear_man ;
float x_dir, y_dir, aspect_ratio, bullet_xdir[50], bullet_ydir[50], bullet_x[50], bullet_y[50] ;
int maxx, maxy, bullet_present[50], score, x_left[32], x_right[32], finish ;

/* starting x coordinates of helicopters which change as the helicopters move */
int heli_x[8] = { 35, 605, 35, 605, 35, 605, 35, 605 } ;

/* level of helicopters, 7 and 21 being their y coordinates */
int heli_level[8] = { 7, 21, 7, 21, 7, 21, 7, 21 } ;

/* number of paratroopers in each helicopter which would reduce as paratroopers jump out */
int no_para[8] = { 4, 4, 4, 4, 4, 4, 4, 4 } ;

/* array which determines whether the helicopter has reached its destination
   or not. 0s would become 1s when helicopters reach their destination */
int h_reached[8] = { 0, 0, 0, 0, 0, 0, 0, 0 } ;

/* x coordinates of paratroopers at the point of jumping from helicopters */
int jump_pt[8][4] = {
						53, 235, 405, 587,
						600, 574, 66, 40,
						79, 196, 444, 561,
						548, 522, 118, 92,
						105, 209, 431, 535,
						496, 470, 170, 144,
						131, 222, 418, 509,
						483, 457, 183, 157
					 } ;

/* y coordinates of paratroopers at the point of jumping which change as the paratroopers descend */
int para_y[8][4] = {
						16, 16, 16, 16,
						30, 30, 30, 30,
						16, 16, 16, 16,
						30, 30, 30, 30,
						16, 16, 16, 16,
						30, 30, 30, 30,
						16, 16, 16, 16,
						30, 30, 30, 30
					} ;

/* status of paratroopers: 1 - alive   0 - hit */
int para_alive[8][4] = {
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1
					   } ;

/* position of paratroopers: 1 - still in helicopter  0 - jumped */
int para_in_heli[8][4] = {
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1,
							1, 1, 1, 1
						 } ;

/* position of paratroopers: 1 - landed  0 - air-borne */
int p_reached[8][4] = {
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0,
						0, 0, 0, 0
					  } ;

main()
{
	int h = 0, i, j, k, delay_counter = 0, t_para_alive, keystroke, lmove = 0, rmove = 0, lnum = 0, rnum = 0 ;
	int la = 0, ra = 0, id_flag = -1, angle = 165, reached_left = 0, reached_right = 0 ;

	/* initialise graphic system, draw basic images and save them */
	start() ;

	/* draw gunbase */
	gbase() ;

	/* draw two helicopters */
	putimage ( heli_x[h] - 33, ( heli_level[h] ) - 5, heli1, OR_PUT ) ;
	putimage ( heli_x[h + 1] - 18, ( heli_level[h + 1] ) - 5, heli2, OR_PUT ) ;

	/* draw gun at specific angle */
	gdraw ( angle, 1 ) ;

	while ( !finish )
	{
		/* introduce delay */
		delay ( 60 ) ;

		/* display score */
		gotoxy ( 10, 25 ) ;
		printf ( "Score : %4d", score ) ;

		/* if a key is pressed */
		if ( kbhit() )
		{
			keystroke = testkeys() ;  /* collect the key hit */

			/* left arrow pressed, start moving gun to left */
			if ( keystroke == 75 )
			{
				lmove = 1 ;
				id_flag = 1 ;  /* set up increment/decrement flag */
			}

			/* right arrow pressed, start moving gun right */
			if ( keystroke == 77 )
			{
				rmove = 1 ;
				id_flag = -1 ;  /* set up increment/decrement flag */
			}

			/* Enter key hit, stop gun & fire bullet */
			if ( keystroke == 13 )
			{
				lmove = 0 ;
				rmove = 0 ;

				/* search the unfired bullet */
				for ( k = 0 ; k < 50 ; k++ )
				{
					/* if bullet has already been fired */
					if ( bullet_present[k] )
						continue ;
					else
					{
						/* set up direction and coordinates of bullet being fired */
						bullet_xdir[k] = x_dir ;
						bullet_ydir[k] = y_dir ;
						bullet_x[k] = 320 + 45 * x_dir ;
						bullet_y[k] = 165 - aspect_ratio * 45 * y_dir ;
						bullet_present[k] = 1 ;

						/* decrement score since a bullet has been fired */
						score-- ;
						break ;
					}
				}
			}

			fflush ( stdin ) ;
		}

		/* move the fired bullets */
		for ( k = 0 ; k < 50 ; k++ )
		{
			/* if bullet has not been fired, search for next bullet */
			if ( !bullet_present[k] )
				continue ;

			/* check whether bullet has crossed the boundary */
			if ( bullet_x[k] >= maxx - 2 || bullet_x[k] <= 2 ||
					bullet_y[k] >= maxy - 2 || bullet_y[k] <= 2 )
				bullet_present[k] = 0 ;

			/* draw the fired bullet at new position */
			if ( bullet_present[k] )
				bdraw ( k, 1 ) ;
		}

		/* if helicopters exist */
		if ( h < 7 )
		{
			/* if the pair of helicopters have not reached the destination */
			if ( !h_reached[h] || !h_reached[h + 1] )
			{
				sound ( 5320 ) ;

				for ( i = 0 ; i < 4 ; i++ )
				{
					/* if paratrooper is not in helicopter or paratrooper has been hit */
					if ( !para_in_heli[h][i] || !para_alive[h][i] )
						continue ;
					else
					{
						/* if helicopter has reached beyond the jumping
						   point of paratrooper, eject the paratrooper */
						if ( heli_x[h] >= jump_pt[h][i] )
						{
							jump_pt[h][i] = heli_x[h] ;
							putimage ( jump_pt[h][i] - 3, para_y[h][i] - 2,
										para, XOR_PUT ) ;
							para_in_heli[h][i] = 0 ;
						}
					}
				}

				/* if helicopter has passed or it has been hit */
				/* whenever a helicopter is hit, the number of paratroopers in it is set to 0 */
				if ( heli_x[h] > maxx - 50 || no_para[h] == 0 )
				{
					h_reached[h]++ ;

					/* erase helicopter from last position */
					if ( h_reached[h] == 1 )
						putimage ( heli_x[h] - 33, ( heli_level[h] ) - 5,
									heli1, XOR_PUT ) ;
				}
				else
					hmove ( h ) ;  /* move the helicopter */


				for ( i = 0 ; i < 4 ; i++ )
				{
					if ( ! ( para_in_heli[h + 1][i] ) ||
						para_alive[h + 1][i] )
						continue ;
					else
					{
						if ( heli_x[h + 1] <= jump_pt[h + 1][i] )
						{
							jump_pt[h + 1][i] = heli_x[h + 1] ;
							putimage ( jump_pt[h + 1][i] - 3,
							 		para_y[h + 1][i] - 2, para, XOR_PUT) ;
							para_in_heli[h + 1][i] = 0 ;
						}
					}
				}

				if ( heli_x[h + 1] < 50 || no_para[h + 1] == 0 )
				{
					h_reached[h + 1]++ ;
					if ( h_reached[h + 1] == 1 )
						putimage ( heli_x[h + 1] - 18, ( heli_level[h + 1] ) - 5, heli2, XOR_PUT ) ;
				}
				else
					hmove ( h + 1 ) ;
			}
			else
			{
				delay_counter++ ;  /* increment delay counter */

				/* the delay ensures that the new pair of helicopters do not start off immediately */

				/* check whether new pair of helicopters should start off */
				if ( delay_counter == 30 )
				{
					delay_counter = 0 ;

					/* if 4 paratroopers haven't reached on either side of gun */
					if ( !la && !ra )
					{
						h += 2 ;

						/* if helicopters have not been exhausted, start the next pair */
						if ( h < 8 )
						{
							putimage ( heli_x[h] - 33, ( heli_level[h] ) - 5, heli1, OR_PUT ) ;
							putimage ( heli_x[h + 1] - 18, ( heli_level[h + 1] ) - 5, heli2, OR_PUT ) ;
						}
					}
				}
			}
		}

		if ( lmove || rmove )
		{
			/* erase the gun */
			gdraw ( angle, 0 ) ;

			angle += ( id_flag * 15 ) ;

			/* if the gun has reached its extreme left position, stop the gun */
			if ( angle >= 165 )
			{
				angle = 165 ;
				lmove = 0 ;
			}

			/* if the gun has reached its extreme right position, stop the gun */
			if ( angle <= 15 )
			{
				angle = 15 ;
				rmove = 0 ;
			}

			/* draw the gun */
			gdraw ( angle, 1 ) ;
		}

		/* for each bullet */
		for ( k = 0 ; k < 50 ; k++ )
		{
			if ( !bullet_present[k] )
				continue ;

			/* erase the bullet */
			bdraw ( k, 0 ) ;

			/* increment the coordinates of the bullet */
			bullet_x[k]+= ( 6 * bullet_xdir[k] ) ;
			bullet_y[k]-= ( 6 * aspect_ratio * bullet_ydir[k] ) ;
		}

		t_para_alive = 0 ;

		/* monitor movement of each paratrooper of each helicopter */
		for ( i = 0 ; i < 8 ; i++ )
		{
			for ( j = 0 ; j < 4 ; j++ )
			{
				/* go to the next paratrooper if not air-borne */
				if ( !para_alive[i][j] || para_in_heli[i][j] || p_reached[i][j] )
					continue ;

				/* if paratrooper has landed */
				if ( para_y[i][j] > maxy - 18 )
				{
					p_reached[i][j]++ ;

					if ( p_reached[i][j] == 1 )
					{
						/* erase paratrooper and parachute */
						putimage ( jump_pt[i][j] - 12, para_y[i][j] - 16, parachute, XOR_PUT ) ;
						putimage ( jump_pt[i][j] - 3, para_y[i][j] - 2, para, XOR_PUT ) ;

						para_y[i][j] = maxy - 17 ;

						/* draw paratrooper */
						putimage ( jump_pt[i][j] - 3, para_y[i][j] - 2, para, OR_PUT ) ;

						/* if paratrooper has landed to the left of gun */
						if ( jump_pt[i][j] < 300 )
						{
							reached_left++ ;

							/* save x coordinate of paratrooper */
							x_left[lnum] = jump_pt[i][j] ;
							lnum++ ;
						}
						else
						{
							reached_right++ ;
							x_right[rnum] = jump_pt[i][j] ;
							rnum++ ;
						}
					}

					/* if four or more paratroopers have reached left of gun
					   and the attack on the gun has not begun from right */
					if ( reached_left >= 4 && !ra )
					{
						/* sort the x coordinates of paratroopers who have reached the left of gun */
						sort ( x_left, reached_left, 0 ) ;
						la = 1 ;
					}
					else
					{
						/* if four or more paratroopers have reached right of gun */
						if ( reached_right >= 4 )
						{
							/* sort the x coordinates of paratroopers who have reached the right of gun */
							sort ( x_right, reached_right, 1 ) ;
							ra = 1 ;
						}
					}
				}
				else
				{
					t_para_alive++ ;  /* count no. of paratroopers alive */
					para_move ( i, j ) ;  /* move the paratrooper */
				}
			}
		}

		/* if four men have landed on left, attack from left */
		if ( la )
			l_attack() ;

		/* if four men have landed on right, attack from right */
		if ( ra )
			r_attack() ;

		/* if all paratroopers have been hit, all helicopters have vanished and
		   the attack has not begun from either side of the gun */
		if ( ( !t_para_alive && ( h >= 7 ) && (!la) && (!ra) ) )
		{
			h = 0 ;

			reset() ;  /* initialise all the variables and restart the game */

			/* display two helicopters */
			putimage ( heli_x[h] - 33, ( heli_level[h] ) - 5, heli1, OR_PUT ) ;
			putimage ( heli_x[h + 1] - 18, ( heli_level[h + 1] ) - 5, heli2, OR_PUT ) ;
		}
		nosound() ;
	}

	/* game finished blast the gun */
	gunblast() ;
	music() ;
	nosound() ;

	/* wait for a keypress */
	if ( kbhit() )
		testkeys() ;
	gotoxy ( 33, 24 ) ;
	getch() ;

	/* shutdown graphic system and restore original VDU mode */
	closegraph() ;
	restorecrtmode() ;
}

/* initialises graphics system, draws basic images and saves them */
start()
{
	int gm = CGAHI, gd = CGA, key = 0, area ;

	initgraph ( &gd, &gm, "c:\\tc\\bgi" ) ;

	/* save maximum values of x and y coordinates */
	maxx = getmaxx() ;
	maxy = getmaxy() ;

	/* draw outer rectangle and set up working area */
	rectangle ( 0, 0, maxx, maxy - 10 ) ;
	setviewport ( 1, 1, maxx - 1, maxy - 11, 1 ) ;

	helidraw ( 50, 50, 0 ) ;  /* draw first helicopter */
	helidraw ( 150, 50, 1 ) ;  /* draw second helicopter */
	pman ( 50, 80 ) ;  /* draw paratrooper */
	paradraw ( 50, 80 ) ;  /* draw parachute */
	pman ( 150, 80 ) ;  /* draw paratrooper */
	paradraw ( 150, 80 ) ;  /* draw parachute */
	gbase() ;  /* draw gun base */
	draw_blast ( 300, 130 ) ;  /* draw the blast image */

	/* calculate and allocate RAM required for storing various images */
	area = imagesize ( 0, 0, 51, 10 ) ;
	heli1 = malloc ( area ) ;
	heli2 = malloc ( area ) ;
	area = imagesize ( 0, 0, 24, 13 ) ;
	parachute = malloc ( area ) ;
	area = imagesize ( 0, 0, 6, 7 ) ;
	para = malloc ( area ) ;
	area = imagesize ( 0, 0, 40, 9 ) ;
	gunbase = malloc ( area ) ;
	area = imagesize ( 0, 0, 32, 12 ) ;
	blast = malloc ( area ) ;
	clear = malloc ( area ) ;
	area = imagesize ( 0, 0, 14, 10 ) ;
	blast_man = malloc ( area ) ;
	clear_man = malloc ( area ) ;

	/* if memory allocation fails */
	if ( heli1 == NULL || heli2 == NULL || parachute == NULL || para == NULL
		|| gunbase == NULL || blast == NULL || clear == NULL ||
			blast_man == NULL || clear_man == NULL )
	{
		printf( "\nInsufficient memory... Press any key " ) ;
		getch() ;
		closegraph() ;
		restorecrtmode() ;
		exit( 0 ) ;
	}

	/* store images in allocated memory */
	getimage( 17, 45, 68, 55, heli1 ) ;
	getimage( 132, 45, 183, 55, heli2 ) ;
	getimage( 38, 64, 62, 77, parachute ) ;
	getimage( 47, 78, 53, 85, para ) ;
	getimage ( 300, 158, 340, 177, gunbase ) ;
	getimage ( 300 - 16, 100 - 6, 300 + 16, 100 + 6, clear ) ;
	getimage ( 330 - 7, 100 - 5, 330 + 7, 100 + 5, clear_man ) ;
	getimage ( 300 - 16, 130 - 6, 300 + 16, 130 + 6, blast ) ;
	getimage ( 330 - 7, 130 - 5, 330 + 7, 130 + 5, blast_man ) ;

	/* set font, alignment and character size */
	settextstyle ( GOTHIC_FONT, HORIZ_DIR, 4 ) ;
	settextjustify ( CENTER_TEXT, CENTER_TEXT ) ;
	outtextxy ( 320, 82, "PARATROOPER" ) ;

	setviewport ( 0, 0, maxx, maxy, 1 ) ;
	settextstyle ( DEFAULT_FONT, HORIZ_DIR, 0 ) ;
	settextjustify ( CENTER_TEXT, TOP_TEXT ) ;
	outtextxy ( 39 * 8, 24 * 8, "Press I for Instructions or P for play" ) ;

	/* continue till either the alphabet I or P is hit */
	while ( key != 'p' && key != 'P' && key != 'i' && key != 'I' )
	{
		while ( !kbhit() ) ;
		key = testkeys() ;
	}

	if ( key == 'i' || key == 'I' )
		message() ;  /* display game instructions */

	clearviewport() ;

	rectangle ( 0, 0, maxx, maxy - 10 ) ;
	setviewport ( 1, 1, maxx - 1, maxy - 11, 1 ) ;
}

/* draws helicopter at specified coordinates pointing in specified direction */
helidraw ( int x, int y, int d )
{
	int direction, i, j ;

	if ( d )
		direction = -1 ;
	else
		direction = 1 ;

	i = 3 ;
	j = 8 ;

	line ( x - j - 8, y - i - 2, x + j + 8, y - i - 2 ) ;
	line ( x - j + 5, y - i - 1, x + j - 5, y - i - 1 ) ;
	line ( x - j, y - i, x + j, y - i ) ;
	for ( ; i > 0 ; i--, j += 2 )
	{
		putpixel ( x - ( direction * j ), y - i, 1 ) ;
		line ( x + ( direction * j ), y - i, x + ( direction * ( j - 8 ) ), y - i ) ;
	}

	i = 0 ;
	j -= 2 ;

	line ( x - ( direction * j ), y - i, x - ( direction * ( j + 17 ) ), y - i ) ;
	line ( x - ( direction * j ), y - i + 1, x - ( direction * ( j + 7 ) ), y - i + 1 ) ;
	putpixel ( x - ( direction * ( j + 19 ) ), y - i - 1, 1 ) ;

	for ( ; i < 3 ; i++, j -= 2 )
	{
		putpixel ( x - j, y + i, 1 ) ;
		putpixel ( x + j, y + i, 1 ) ;
	}

	line ( x - j, y + i, x + j, y + i ) ;
	putpixel ( x - j + 3, y + i + 1, 1 ) ;
	putpixel ( x + j - 3, y + i + 1, 1 ) ;
	line ( x - j - 10, y + i + 2, x + j + 10, y + i + 2 ) ;
	putpixel ( x + ( direction * ( j + 12 ) ), y + i + 1, 1 ) ;
}

/* draws paratrooper at specified position */
pman ( int x, int y )
{
	rectangle ( x - 1, y - 2, x + 1, y + 2 ) ;
	line ( x, y - 2, x, y + 2 ) ;
	line ( x - 3, y, x + 3, y ) ;
	line ( x - 3, y + 3, x - 3, y + 5 ) ;
	line ( x + 3, y + 3, x + 3, y + 5 ) ;
}

/* draws parachute at specified position */
paradraw ( int x, int y )
{
	ellipse ( x, y - 11, 0, 180, 12, 5 ) ;
	line ( x - 12, y - 11, x + 12, y - 11 ) ;
	floodfill ( x, y - 12, 1 ) ;
	line ( x - 12, y - 10, x - 3, y - 3 ) ;
	line ( x + 12, y - 10, x + 3, y - 3 ) ;
}

/* draws gun base */
gbase()
{
	rectangle ( 280, 171, 360, 189 ) ;
	floodfill ( 282, 173, 1 ) ;
	line ( 300, 165, 300, 171 ) ;
	line ( 340, 165, 340, 171 ) ;
	ellipse ( 320, 165, 0, 180, 20, 7 ) ;
	floodfill ( 310, 169, 1 ) ;
}

/* draws or erases gun at given angle */
gdraw ( int angle, int color )
{
	int oy = 165, lx = 315, rx = 325, r = 40 ;
	float x1, x2, ly, ry, y1, y2 ;

	setcolor ( color ) ;
	aspect_ratio = 5.0 / 16.0 ;
	x_dir = cos ( 3.14 / 180 * angle ) ;
	y_dir = sin ( 3.14 / 180 * angle ) ;

	x1 = lx + r * x_dir ;
	x2 = rx + r * x_dir ;

	ly = oy - aspect_ratio * 4 * sin ( 3.14 / 180 * ( angle + 90 ) ) ;
	ry = oy - aspect_ratio * 4 * sin ( 3.14 / 180 * ( angle - 90 ) ) ;
	y1 = ly - aspect_ratio * r * y_dir ;
	y2 = ry - aspect_ratio * r * y_dir ;

	line ( lx, ly, x1, y1 ) ;
	line ( rx, ry, x2, y2 ) ;
	line ( x1, y1, x2, y2 ) ;
	line ( lx, ly, rx, ry ) ;
	putimage ( 300, 158, gunbase, OR_PUT ) ;
	setcolor ( WHITE ) ;
}

/* draws or erases bullet */
bdraw ( int bno, int color )
{
	setcolor ( color ) ;
	putpixel ( bullet_x[bno], bullet_y[bno], color ) ;
	setcolor ( WHITE ) ;
}

/* moves the specified helicopter */
hmove ( int hno )
{
	/* if the helicopter which moves from left to right */
	if ( hno % 2 == 0 )
	{
		putimage ( heli_x[hno] - 33, ( heli_level[hno] ) - 5, heli1, XOR_PUT ) ;

		/* if the helicopter is hit by bullet */
		if ( check_h ( hno ) )
		{
			score += 20 ;
			return ;
		}

		heli_x[hno] += 6 ;
		putimage ( heli_x[hno] - 33, ( heli_level[hno] ) - 5, heli1, OR_PUT ) ;
	}
	else
	{
		putimage ( heli_x[hno] - 18, ( heli_level[hno] ) - 5, heli2, XOR_PUT ) ;

		if ( check_h ( hno ) )
		{
			score += 20 ;
			return ;
		}

		heli_x[hno] -= 6 ;
		putimage ( heli_x[hno] - 18, ( heli_level[hno] ) - 5, heli2, OR_PUT ) ;
	}
}

/* moves the specified paratrooper */
para_move ( int h, int i )
{
	if ( para_y[h][i] > 100 )
		putimage ( jump_pt[h][i] - 12, para_y[h][i] - 16, parachute, XOR_PUT ) ;
	putimage ( jump_pt[h][i] - 3, para_y[h][i] - 2, para, XOR_PUT ) ;

	/* if the paratrooper is hit by bullet */
	if ( check_m ( h, i ) )
	{
		score += 10 ;
		return ;
	}

	/* if the parachute is hit by bullet */
	if ( ( para_y[h][i] > 100 ) && check_p ( h, i ) )
	{
		score += 10 ;
		return ;
	}

	para_y[h][i] += 2 ;

	/* if paratrooper's y coordinate exceeds 100, draw parachute */
	if ( para_y[h][i] > 100 )
		putimage ( jump_pt[h][i] - 12, para_y[h][i] - 16, parachute, OR_PUT ) ;

	/* draw paratrooper at new position */
	putimage ( jump_pt[h][i] - 3, para_y[h][i] - 2, para, OR_PUT ) ;
}

/* checks whether helicopter is hit by bullet or not */
check_h ( int hno )
{
	int j, k, c1 = 0, c2 = 0, c3 = 0, c4 = 0 ;

	for ( k = 0 ; k < 50 ; k++ )
	{
		if ( !bullet_present[k] )
			continue ;

		/* check if body of helicopter is hit */
		c1 = ( ( bullet_y[k] >= heli_level[hno] - 5 ) && ( bullet_y[k] <= heli_level[hno] + 5 ) ) ;
		c2 = ( ( bullet_x[k] >= heli_x[hno] - 18 ) && ( bullet_x[k] <= heli_x[hno] + 18 ) ) ;

		/* check if tail of helicopter is hit */
		c3 = ( ( bullet_y[k] >= heli_level[hno] ) && ( bullet_y[k] <= heli_level[hno] + 1 ) ) ;
		if ( ( hno % 2 ) )
			c4 = ( ( bullet_x[k] >= heli_x[hno] + 18 ) && ( bullet_x[k] <= heli_x[hno] + 33 ) ) ;
		else
			c4 = ( ( bullet_x[k] >= heli_x[hno] - 33 ) && ( bullet_x[k] <= heli_x[hno] - 18 ) ) ;

		if ( ( c1 && c2 ) || ( c3 && c4 ) )
		{
			no_para[hno] = 0 ;
			h_reached[hno] = 1 ;
			for ( j = 0 ; j < 4 ; j++ )
			{
				/* if paratrooper is inside the helicopter that has been hit */
				if ( para_in_heli[hno][j] )
					para_alive[hno][j] = 0 ;
			}

			/* stop the movement of the bullet that has hit the helicopter */
			bullet_present[k] = 0 ;
			bdraw ( k, 0 ) ;  /* erase that bullet */

			/* display blast image */
			putimage ( heli_x[hno], heli_level[hno], blast, OR_PUT ) ;

			sound ( 250 ) ;
			delay ( 500 ) ;
			nosound() ;

			/* clear blast image */
			putimage ( heli_x[hno], heli_level[hno], clear, COPY_PUT ) ;

			return ( 1 ) ;  /* report that a blast has occurred */
		}
	}
	return ( 0 ) ;  /* report that a blast hasn't occurred */
}

/* checks whether paratrooper from the specified helicopter is hit by bullet or not */
check_m ( int hno, int pno )
{
	int k, c1 = 0, c2 = 0 ;

	for ( k = 0 ; k < 50 ; k++ )
	{
		if ( !bullet_present[k] )
			continue ;

		c1 = ( ( bullet_y[k] >= para_y[hno][pno] - 2 ) && ( bullet_y[k] <= para_y[hno][pno] + 6 ) ) ;
		c2 = ( ( bullet_x[k] >= jump_pt[hno][pno] - 3 ) && ( bullet_x[k] <= jump_pt[hno][pno] + 3 ) ) ;

		if ( c1 && c2 )
		{
			para_alive[hno][pno] = 0 ;

			/* stop the movement of the bullet that has hit the helicopter */
			bullet_present[k] = 0 ;
			bdraw ( k, 0 ) ;  /* erase that bullet */

			/* display blast image */
			putimage ( jump_pt[hno][pno], para_y[hno][pno] + 2, blast_man, OR_PUT ) ;

			sound ( 500 ) ;
			delay ( 500 ) ;
			nosound() ;

			/* erase blast image */
			putimage ( jump_pt[hno][pno], para_y[hno][pno] + 2, clear_man, COPY_PUT ) ;

			return ( 1 ) ;  /* report that a blast has occurred */
		}
	}
	return ( 0 ) ;  /* report that a blast hasn't occurred */
}

/* checks whether the parachute has been hit by the bullet or not */
check_p ( int hno, int pno )
{
	int k, c1 = 0, c2 = 0 ;

	for ( k = 0 ; k < 50 ; k++ )
	{
		if ( !bullet_present[k] )
			continue ;

		c1 = ( ( bullet_y[k] >= para_y[hno][pno] - 16 ) && ( bullet_y[k] <= para_y[hno][pno] -3 ) ) ;
		c2 = ( ( bullet_x[k] >= jump_pt[hno][pno] - 12 ) && ( bullet_x[k] <= jump_pt[hno][pno] + 12 ) ) ;

		if ( c1 && c2 )
		{
			para_alive[hno][pno] = 0 ;

			/* stop the movement of the bullet that has hit the helicopter */
			bullet_present[k] = 0 ;
			bdraw ( k, 0 ) ;  /* erase that bullet */

			/* display the blast image */
			putimage ( jump_pt[hno][pno], para_y[hno][pno] - 7, blast_man, OR_PUT ) ;

			sound ( 500 ) ;
			delay ( 500 ) ;
			nosound() ;

			/* erase the blast image */
			putimage ( jump_pt[hno][pno], para_y[hno][pno] - 7, clear_man, COPY_PUT ) ;

			return ( 1 ) ;  /* report that a blast has occurred */
		}
	}
	return ( 0 ) ;  /* report that a blast hasn't occurred */
}

/* returns the ascii or scan code of the key hit */
testkeys()
{
	union REGS ii, oo ;

	ii.h.ah = 0 ;
	int86 ( 22, &ii, &oo ) ;

	/* if ascii code is not 0 */
	if ( oo.h.al )
		return ( oo.h.al ) ;  /* return ascii code */
	else
		return ( oo.h.ah ) ;  /* return scan code */
}

/* sorts the x coordinates of the paratroopers who have landed */
sort ( int *spx, int count, int f )
{
	int tmp, i, j ;

	/* if paratroopers on left of gun are being considered then sort
	   x coordinates in descending order otherwise in ascending order */
	for ( i = 0 ; i < count - 1 ; i++ )
	{
		for ( j = i + 1 ; j < count ; j++ )
		{
			if ( f )
			{
				if ( spx[j] < spx[i] )
				{
					tmp = spx[j] ;
					spx[j] = spx[i] ;
					spx[i] = tmp ;
				}
			}
			else
			{
				if ( spx[j] > spx[i] )
				{
					tmp = spx[j] ;
					spx[j] = spx[i] ;
					spx[i] = tmp ;
				}
			}
		}
	}
}

/* carries out the attack from right */
r_attack()
{
	static int i, k, r = 365 ;

	/* move first two paratroopers upto the gun base */
	while( k <= 1 )
	{
		while ( x_right[k] > r )
		{
			/* erase paratrooper from existing position */
			putimage ( x_right[k] - 3, maxy - 17 - 2, para, XOR_PUT ) ;

			x_right[k] -= 2 ;

			/* draw paratrooper at new position */
			putimage ( x_right[k] - 3, maxy - 17 - 2, para, OR_PUT ) ;
			return ;
		}
		k++ ;
		r += 10 ;
	}

	/* if first two paratroopers have reached upto the gun base */
	if ( !i )
	{
		k-- ;
		r -= 10 ;

		/* place one paratrooper on the head of another */
		putimage ( x_right[k] - 3, maxy - 17 - 2, para, XOR_PUT ) ;
		putimage ( x_right[k - 1] - 3, maxy - 17 - 10, para, OR_PUT ) ;

		i++ ;
		k++ ;
		return ;
	}

	/* move third and fourth paratroopers upto the first one */
	while ( k <= 3 )
	{
		while ( x_right[k] > r )
		{
			putimage ( x_right[k] - 3, maxy - 17 - 2, para, XOR_PUT ) ;
			x_right[k] -= 2 ;
			putimage ( x_right[k] - 3, maxy - 17 - 2, para, OR_PUT ) ;
			return ;
		}

		k++ ;
		r += 10 ;
	}

	/* if three paratroopers have already reached gun base */
	if ( i == 1 )
	{
		k-- ;

		/* place fourth paratrooper on the head of third */
		putimage ( x_right[k] - 3, maxy - 17 - 2, para, XOR_PUT ) ;
		putimage ( x_right[k - 1] - 3, maxy - 17 - 10, para, OR_PUT ) ;
		k++ ;
		i++ ;
		return ;
	}

	/* if fourth paratrooper has climbed on the head of third */
	if ( i == 2 )
	{
		k-- ;

		/* erase fourth paratrooper who is standing on the head of third */
		putimage ( x_right[k - 1] - 3, maxy - 17 - 10, para, XOR_PUT ) ;

		/* place fourth paratrooper on the head of second */
		putimage ( x_right[k - 3] - 3, maxy - 17 - 20, para, OR_PUT ) ;

		delay ( 400 ) ;

		/* erase fourth paratrooper who is standing on the head of second */
		putimage ( x_right[k - 3] - 3, maxy - 17 - 20, para, XOR_PUT ) ;

		/* let fourth paratrooper climb on the gun */
		putimage ( x_right[k - 3] - 3 - 10, maxy - 17 - 20, para, OR_PUT ) ;

		finish = 1 ;  /* gun is about to be blasted, game over */
		return ;
	}
}

/* carries out the attack from left */
l_attack()
{
	static int i, k, r = 275 ;

	/* move first two paratroopers upto the gun base */
	while( k <= 1 )
	{
		while ( x_left[k] < r )
		{
			/* erase paratrooper from existing position */
			putimage ( x_left[k] - 3, maxy - 17 - 2, para, XOR_PUT ) ;

			x_left[k] += 2 ;

			/* draw paratrooper at new position */
			putimage ( x_left[k] - 3, maxy - 17 - 2, para, OR_PUT ) ;
			return ;
		}
		k++ ;
		r -= 10 ;
	}

	/* if first two paratroopers have reached upto the gun base */
	if ( !i )
	{
		k-- ;
		r += 10 ;

		/* place one paratrooper on the head of another */
		putimage ( x_left[k] - 3, maxy - 17 - 2, para, XOR_PUT ) ;
		putimage ( x_left[k - 1] - 3, maxy - 17 - 10, para, OR_PUT ) ;

		i++ ;
		k++ ;
		return ;
	}

	/* move third and fourth paratroopers upto the first one */
	while ( k <= 3 )
	{
		while ( x_left[k] < r )
		{
			putimage ( x_left[k] - 3, maxy - 17 - 2, para, XOR_PUT ) ;
			x_left[k] += 2 ;
			putimage ( x_left[k] - 3, maxy - 17 - 2, para, OR_PUT ) ;
			return ;
		}
		k++ ;
		r -= 10 ;
	}

	/* if three paratroopers have already reached gun base */
	if ( i == 1 )
	{
		k-- ;
		putimage ( x_left[k] - 3, maxy - 17 - 2, para, XOR_PUT ) ;
		putimage ( x_left[k - 1] - 3, maxy - 17 - 10, para, OR_PUT ) ;
		k++ ;
		i++ ;
		return ;
	}

	/* if fourth paratrooper has climbed on the head of third */
	if ( i == 2 )
	{
		k-- ;

		/* erase fourth paratrooper who is standing on the head of third */
		putimage ( x_left[k - 1] - 3, maxy - 17 - 10, para, XOR_PUT ) ;

		/* place fourth paratrooper on the head of second */
		putimage ( x_left[k - 3] - 3, maxy - 17 - 20, para, OR_PUT ) ;

		delay ( 400 ) ;

		/* erase fourth paratrooper who is standing on the head of second */
		putimage ( x_left[k - 3] - 3, maxy - 17 - 20, para, XOR_PUT ) ;

		/* let fourth paratrooper climb on the gun */
		putimage ( x_left[k - 3] - 3 + 10, maxy - 17 - 20, para, OR_PUT ) ;

		finish = 1 ;  /* gun is about to be blasted, game over */
		return ;
	}
}

/* draws the blast image */
draw_blast ( int x, int y )
{
	line ( x - 16, y - 6, x - 10, y - 2 ) ;
	line ( x - 10, y - 2, x - 16, y ) ;
	line ( x - 16, y, x - 10, y + 2 ) ;
	line ( x - 10, y + 3, x - 16, y + 6 ) ;

	line ( x - 16, y + 6, x - 4, y + 2 ) ;
	line ( x - 4, y + 2, x, y + 6 ) ;
	line ( x, y + 6, x + 4, y + 2 ) ;
	line ( x + 4, y + 2, x + 16, y + 6 ) ;

	line ( x - 16, y - 6, x - 4, y - 2 ) ;
	line ( x - 4, y - 2, x, y - 6 ) ;
	line ( x, y - 6, x + 4, y - 2 ) ;
	line ( x + 4, y - 2, x + 16, y - 6 ) ;

	line ( x + 16, y - 6, x + 10, y - 2 ) ;
	line ( x + 10, y - 2, x + 16, y ) ;
	line ( x + 16, y, x + 10, y + 2 ) ;
	line ( x + 10, y + 2, x + 16, y + 6 ) ;

	x += 30 ;

	line ( x - 7, y - 5, x - 3, y - 2 ) ;
	line ( x - 3, y - 2, x - 7, y ) ;
	line ( x - 7, y, x - 3, y + 2 ) ;
	line ( x - 3, y + 3, x - 7, y + 5 ) ;

	line ( x - 7, y + 5, x - 2, y + 2 ) ;
	line ( x - 2, y + 2, x, y + 5 ) ;
	line ( x, y + 5, x + 2, y + 2 ) ;
	line ( x + 2, y + 2, x + 7, y + 5 ) ;

	line ( x - 7, y - 5, x - 2, y - 2 ) ;
	line ( x - 2, y - 2, x, y - 5 ) ;
	line ( x, y - 5, x + 2, y - 2 ) ;
	line ( x + 2, y - 2, x + 7, y - 5 ) ;

	line ( x + 7, y - 5, x + 3, y - 2 ) ;
	line ( x + 3, y - 2, x + 7, y ) ;
	line ( x + 7, y, x + 3, y + 2 ) ;
	line ( x + 3, y + 2, x + 7, y + 5 ) ;
}

/* blasts the gun at the end of the game */
gunblast()
{
	void *a1, *a2, *a3, *a4 ;
	int area, i ;

	area = imagesize ( 0, 0, 10, 2 ) ;
	a1 = malloc ( area ) ;
	a2 = malloc ( area ) ;
	a3 = malloc ( area ) ;
	a4 = malloc ( area ) ;

	getimage ( 310, 173, 320, 175, a1 ) ;
	getimage ( 325, 173, 335, 175, a2 ) ;
	getimage ( 315, 168, 325, 170, a3 ) ;
	getimage ( 330, 165, 340, 167, a4 ) ;

	putimage ( 304, 158, blast, COPY_PUT ) ;

	for ( i = 0 ; i < 40 ; i += 2 )
	{
		putimage ( 310 - i * 3, 177 - i * 2, a1, XOR_PUT ) ;
		putimage ( 325 + i * 3, 177 - i * 2, a2, XOR_PUT ) ;

		putimage ( 310 - i , 177 - i * 2, a1, XOR_PUT ) ;
		putimage ( 325 + i , 177 - i * 2, a2, XOR_PUT ) ;

		putimage ( 310 - i * 4, 177 - i * 2, a1, XOR_PUT ) ;
		putimage ( 325 + i * 4, 177 - i * 2, a2, XOR_PUT ) ;

		putimage ( 310 - i * 7, 177 - i * 2, a1, XOR_PUT ) ;
		putimage ( 325 + i * 7, 177 - i * 2, a2, XOR_PUT ) ;

		putimage ( 315 - i * 3, 170 - i * 2, a1, XOR_PUT ) ;
		putimage ( 330 + i * 3, 167 - i * 2, a2, XOR_PUT ) ;
	}
}

/* initialises the variables and restarts the game */
reset()
{
	int i, j, k ;

	for ( i = 0 ; i < 8 ; i++ )
	{
		if ( i % 2 )
		{
			k = 30 ;
			heli_x[i] = 605 ;
		}
		else
		{
			k = 16 ;
			heli_x[i] = 35 ;
		}
		no_para[i] = 4 ;
		h_reached[i] = 0 ;

		for ( j = 0 ; j < 4 ; j++ )
		{
			para_y[i][j] = k ;
			para_alive[i][j] = 1 ;
			para_in_heli[i][j] = 1 ;
			p_reached[i][j] = 0 ;
		}
	}
}

/* displays the instructions of the game */
message()
{
	int sp = 0 ;

	clearviewport() ;

	gotoxy ( 31, 2 ) ;
	printf ( "* Your Mission *" ) ;
	gotoxy ( 4, 4 ) ;
	printf ( "Do not allow enemy paratroopers to land on either side of your gunbase." ) ;
	gotoxy ( 4, 5 ) ;
	printf ( "If four paratroopers land on one side of your base, they will overpower" ) ;
	gotoxy ( 4, 6 ) ;
	printf ( "your defenses and blow up your gun.  " ) ;
	gotoxy ( 4, 8 ) ;
	printf ( "The arrow keys & Enter key control your gun and firing of your bullets." ) ;
	gotoxy ( 10, 10 ) ;
	printf ( "<-  Counter clock wise            ->  Clock wise " ) ;

	gotoxy ( 4, 12 ) ;
	printf ( "Using Enter key, stop the movement of the gun and fire bullets. " ) ;

	gotoxy ( 15, 15 ) ;
	printf ( "If you hit a HELICOPTER  --   20 points" ) ;
	gotoxy ( 15, 17 ) ;
	printf ( "If you hit a PARATROOPER --   10 points" ) ;
	gotoxy ( 15, 19 ) ;
	printf ( "Each bullet fired costs you 1 point" ) ;
	gotoxy ( 4, 21 ) ;
	printf ( "Press <SPACE> to continue..." ) ;

	/* continue till a space is hit */
	while ( sp != 32 )
	{
		while ( !kbhit() ) ;
		sp = testkeys() ;
	}

	clearviewport() ;
}

/* plays music at the end of the game */
music()
{
	int i, j ;

	delay ( 1 ) ;

	for ( j = 0 ; j < 3 ; j++ )
	{
		for ( i = 0 ; i <= 10 ; i++ )
		{
			sound ( 150 + i * 10 ) ;
			delay ( 100 ) ;
		}
		nosound() ;
	}
}
