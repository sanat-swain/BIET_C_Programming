/*  CHART.C  */
/* Refer Appendix B in the book if you want to create standalone
   EXE file of this program during compilation */

# include "charthdr.c"
main()
{
	int mchoice ;

	/* collect the time at which execution of the program begins */
	time ( &starttime ) ;

	/* determine graphics driver by checking the hardware */
	detectgraph ( &gd, &gm ) ;

	/* if hardware doesn't support graphics */
	if ( gd == -2 )
	{
		printf ( "\nThis program requires hardware with graphics capability to operate successfully" ) ;
		printf ( "\nPlease use this program on such a system" ) ;
		printf ( "\nPress any key..." ) ;
		fflush ( stdin ) ;  /* flush the keyboard buffer */
		getch() ;
		activetime() ;  /* calculate and display active time */
		exit ( 1 ) ;  /* terminate program execution */
	}

	/* store base address of VDU memory */
	vid_mem = ( char far * ) 0xb8000000L ;

	/* set video mode to text mode */
	textmode ( 3 ) ;

	/* calculate area required to save entire screen contents in graphics mode */
	area = 4 + ( 640 + 7 ) / 8 * 200 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	graph_ptr = malloc ( area ) ;
	if ( graph_ptr == NULL )
		alloc_error_exit() ;

	/* prepare the screen on which the different menus are popped up */
	size ( 32, 0 ) ;
	menubox ( 0, 0, 24, 79, 47, 0 ) ;
	drawbox ( 0, 0, 24, 79, 47 ) ;
	writestring ( messages[30], 0, 31, 48 ) ;

	while ( 1 )
	{
		/* pop up Main menu and collect choice made by user */
		mchoice = popupmenu ( mainmenu, 4, "DGFE", 0 ) ;

		/* test choice received */
		switch( mchoice)
		{
			case 1 :
				data_options() ;
				break ;

			case 2 :
				graph_options() ;
				break ;

			case 3 :
				file_options() ;
				break ;

			case 4 :
				exit_options() ;
		}
	}
}

/* pops up Data menu, receives choice, branches to appropriate function */
data_options()
{
	int dchoice ;

	while ( 1 )
	{
		dchoice = popupmenu ( datamenu, 3, "FKR", 5 ) ;

		switch ( dchoice )
		{
			case 1 :  /* read data from a file */
				data_from_file() ;
				break ;

			case 2 :  /* receive data from the keyboard */
				data_from_kb() ;
				break ;

			case 3 :  /* return to main menu */
				return ;
		}
	}
}

/* pops up Graph menu, receives choice, branches to appropriate function */
graph_options()
{
	int gchoice ;

	while ( 1 )
	{
		gchoice = popupmenu ( graphmenu, 6, "BSPXLR", 9 ) ;

		switch ( gchoice )
		{
			case 1 :  /* bar  chart */
				bar_chart() ;
				break ;

			case 2 :  /* stacked bar chart */
				stack_bar_chart() ;
				break ;

			case 3 :  /* pie chart */
				pie_chart() ;
				break ;

			case 4 :  /* XY chart */
				xy_or_line ( XY ) ;
				break ;

			case 5 :  /* line chart */
				xy_or_line ( LINE ) ;
				break ;

			case 6 :  /* return to main menu */
				return ;
		}
	}
}

/* pops up File menu, receives choice, branches to appropriate function */
file_options()
{
	int fchoice ;

	while ( 1 )
	{
		fchoice = popupmenu ( filesmenu, 4, "LSPR", 16 ) ;

		switch ( fchoice )
		{
			case 1 :  /* load a graph from a file */
				load() ;
				break ;

			case 2 :  /* save the current graph into a file */
				save() ;
				break ;

			case 3 :  /* print the current graph on the printer */
				print() ;
				break ;

			case 4 :  /* return to main menu */
				return ;
		}
	}
}

/* pops up Exit menu, receives choice, branches to appropriate function */
exit_options()
{
	int fchoice ;

	while ( 1 )
	{
		fchoice = popupmenu ( exitmenu, 3, "ESR", 26 ) ;

		switch ( fchoice )
		{
			case 1 :  /* exit permanently to DOS */
				activetime() ;  /* calculate and display active time */
				exit ( 0 ) ;

			case 2 :  /* exit temporarily to DOS */
				shell() ;
				break ;

			case 3 :  /* return to main menu */
				return ;
		}
	}
}

/* pops up Keyboard Entry menu, receives choice, branches to appropriate function */
data_from_kb()
{
	int kchoice ;

	while ( 1 )
	{
		kchoice = popupmenu ( keyboardmenu, 4, "NELR", 21 ) ;

		switch ( kchoice )
		{
			case 1 :  /* enter new data values */
				newdata() ;
				break ;

			case 2 :  /* edit existing data values */
				editdata() ;
				break ;

			case 3 :  /* enter legend values */
				legendentry() ;
				break ;

			case 4 :  /* return to Data menu */
				return ;
		}
	}
}

/* Initialises the graphics system */
graphicsmode()
{
	int g_error ;

	/* setup graphics driver and mode */
	gd = CGA ;
	gm = CGAHI ;

	/* initialise the graphics system */
	initgraph ( &gd, &gm, "c:\\tc\\bgi" ) ;

	/* determine success/failure of initialisation operation */
	g_error = graphresult() ;

	/* if failure */
	if ( g_error != 0 )
	{
		/* print error message string */
		printf ( "\ninitgraph error: %s.\n", grapherrormsg ( g_error ) ) ;
		exit ( 1 ) ;
	}
}

/* clears the last row on the screen */
cleartext()
{
	int i ;
	for ( i = 1 ; i <= 78 ; i++ )
		writechar ( 23, i, ' ', 32 ) ;
}

/* accepts fresh data from keyboard */
newdata()
{
	double num ;
	int areareqd, i, entry, flag, indicator = 0 ;
	int col[7] = { 13, 24, 35, 46, 57, 68, 2 } ;
	char *p ;

	/* calculate area required to save entire screen contents */
	areareqd = ( 24 - 0 + 1 ) * ( 79 - 0 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	/* save screen contents */
	savevideo ( 0, 0, 24, 79, p ) ;

	/* set up data_type to indicate that data is being entered from keyboard */
	data_type = 4 ;

	/* draw the box used for entry of data */
	draw_entrybox() ;

	/* clear last row and display message */
	cleartext() ;
	writestring ( "Press Enter on empty entry to stop", 23, 1, 32 ) ;
	writestring ( "Esc to abort", 23, 66, 32 ) ;

	/* accept x axis values, maximum 15 */
	for ( i = 0 ; i <= 14 ; i++ )
	{
		/* store the value entered in num; getnum will respond to the Esc key in a special way */
		getnum ( &flag, &num, i, 2, ESCAPE ) ;

		/* if user hits Esc or Enter on an empty entry */
		if ( flag == ENTRYOVER || flag == ESCAPE )
			break ;

		/* assign the number entered to the global variable xrange[] */
		xrange[i] = num ;
	}

	/* store the total number of x axis entries made */
	max_x_entries = i ;

	/* abandon all data entered if Esc key is hit */
	if ( flag == ESCAPE )
		max_x_entries = 0 ;

	/* accept sets of data values, maximum 6 sets */
	for ( i = 0 ; i <= 5 ; i++ )
	{
		/* accept as many y axis values as there are x axis entries */
		for ( entry = 0 ; entry < max_x_entries ; entry++ )
		{
			getnum ( &flag, &num, entry, col[i], ESCAPE ) ;

			/* if Esc key is hit */
			if ( flag == ESCAPE )
			{
				indicator = 1 ;
				break ;
			}

			/* if user hits Enter on a blank entry */
			if ( flag == ENTRYOVER )
			{
				/* if at least one set of y axis values is entered */
				if ( entry == 0 && i != 0 )
				{
					indicator = 1 ;
					break ;
				}

				/* if Enter is hit before supplying even one set of y axis values or at an entry other than the first */
				if ( ( entry == 0 && i == 0 ) || entry != 0 )
				{
					cleartext() ;
					writestring ( "Enter all values!", 23, 1, 32 ) ;
					writestring ( "Press any key to continue...", 23, 47, 32 ) ;
					fflush ( stdin ) ;
					getch() ;
					cleartext() ;
					writestring ( "Press Enter on empty entry to stop", 23, 1, 32 ) ;
					writestring ( "Esc to abort", 23, 66, 32 ) ;
					entry-- ;
					continue ;
				}
			}

			/* assign the received number to the global variable yrange[][] */
			yrange[entry][i] = num ;
		}

		/* if Esc key was hit or if Enter was hit after supplying complete sets of y axis values */
		if ( indicator )
			break ;
	}

	/* store the total number of sets of y axis entries made */
	max_y_sets = i ;

	/* abandon all data entered if Esc key is hit */
	if ( flag == ESCAPE )
	{
		max_y_sets = 0 ;
		max_x_entries = 0 ;
	}

	/* if any entries are made, get them okayed */
	if ( max_x_entries > 0 )
		verifydata() ;

	/* restore original screen contents and free allocated memory */
	restorevideo ( 0, 0, 24, 79, p ) ;
	free ( p ) ;
}

/* draws a box used for entering, editing or displaying data */
draw_entrybox()
{
	int i, j ;

	/* display a filled box */
	menubox ( 0, 0, 24, 79, 47, 0 ) ;

	/* draw a double lined box */
	drawbox ( 0, 0, 24, 79, 47 ) ;
	writestring ( "  Data Entry  ", 0, 33, 48 ) ;

	/* draw the horizontal lines */
	for ( i = 0 ; i <= 2 ; i++ )
	{
		for ( j = 2 ; j <= 77 ; j++ )
		{
			writechar ( 2, j, 196, 32 ) ;
			writechar ( 5, j, 196, 32 ) ;
			writechar ( 22, j, 196, 32 ) ;
		}
	}

	/* draw the vertical lines */
	for ( j = 1 ; j <= 78 ; j += 11 )
	{
		for ( i = 4 ; i <= 21 ; i++ )
			writechar ( i, j, 179, 32 ) ;
	}
	for ( j = 1 ; j <= 13 ; j += 11 )
		writechar ( 3, j, 179, 32 ) ;
	writechar ( 3, 78, 179, 32 ) ;

	/* complete the box by drawing the appropriate characters at the edges */
	i = 0 ;
	for ( j = 17 ; j < 78 ; j += 11 )
	{
		writechar ( 4, j, arr[i], 32 ) ;
		writechar ( 5, j - 5, 197, 32 ) ;
		writechar ( 22, j - 5, 193, 32 ) ;
		i++ ;
	}
	writechar ( 2, 1, 218, 32 ) ;
	writechar ( 22, 1, 192, 32 ) ;
	writechar ( 2, 78, 191, 32 ) ;
	writechar ( 22, 78, 217, 32 ) ;
	writechar ( 2, 12, 194, 32 ) ;
	writechar ( 5, 1, 195, 32 ) ;
	writechar ( 5, 78, 180, 32 ) ;

	writestring ( "X-axis", 3, 4, 32 ) ;
	writestring ( "Values", 4, 4, 32 ) ;
	writestring ( "Y-axis values", 3, 38, 32 ) ;
}

/* accepts a number from the keyboard */
getnum ( int *pf, double *num, int row, int col, int key )
{
	int ch, i = 0 ;
	char str[12] ;

	*pf = ENTRYSTART ;
	size ( 5, 7 ) ;  /* display cursor */

	while ( 1 )
	{
		gotoxy ( col + i + 1, 7 + row ) ;

		/* get a character */
		fflush ( stdin ) ;
		ch = getch() ;

		/* if the key hit is a special key, get the scan code */
		if ( ch == 0 )
			ch = getch() ;

		/* if the key hit is Esc or End as specified by the calling function */
		if ( ch == key )
		{
			*pf = ch ;
			size ( 32, 0 ) ;  /* hide cursor */
			return ;
		}

		/* if the Backspace key is hit */
		if ( ch == '\b' )
		{
			/* if the cursor is not on the first column */
			if ( i != 0 )
			{
				i-- ;

				/* erase previous character */
				writechar ( row + 6, col + i, ' ', 47 ) ;
			}
		}

		/* if the Enter key is hit */
		if ( ch == '\r' )
		{
			/* if the Enter key is hit on a blank entry */
			if ( i == 0 )
			{
				*pf = ENTRYOVER ;  /* set up flag */
				size ( 32, 0 ) ;  /* hide cursor */
				return ;
			}
			else
				break ;
		}

		/* display the character only if it is a digit, a decimal point or a minus sign entered in the first column */
		if ( isdigit ( ch ) || ch == '.' || ( ch == '-' && i == 0 ) )
		{
			/* clear the columns for receiving a fresh entry */
			if ( i == 0 )
				writestring ( "         ", row + 6, col + i, 47 ) ;

			str[i] = ch ;
			writechar ( row + 6, col + i, ch, 47 ) ;
			i++ ;

			/* if the number entered exceeds 10 digits */
			if ( i > 9 )
				break ;
		}
	}

	str[i] = '\0' ;  /* terminate the string */

	/* convert the input string to a floating point number */
	*num = atof ( str ) ;

	size ( 32, 0 ) ;
}

/* gets the data okayed */
verifydata()
{
	int entry, flag, i, areareqd ;
	int col[7] = { 13, 24, 35, 46, 57, 68, 2 } ;
	char ans, *p ;
	FILE *fp ;


	/* so long as data is not okay */
	while ( 1 )
	{
		/* erase the last row, display the message, get user's response */
		cleartext() ;
		writestring ( "Is the data OK (Y/N) ?", 23, 16, 32 ) ;
		fflush ( stdin ) ;  /* flush the keyboard buffer */
		ans = getch() ;

		/* if data is okay, terminate the loop */
		if ( ans == 'Y' || ans == 'y' )
			break ;

		/* erase the last row, display the message */
		cleartext() ;
		writestring ( "Press Enter if current entry is OK.", 23, 1, 32 ) ;
		writestring ( "Press End to finish.", 23, 58, 32 ) ;

		/* get all the x axis entries okayed */
		for ( entry = 0 ; entry < max_x_entries ; entry++ )
		{
			getnum ( &flag, &xrange[entry], entry, col[6], END ) ;

			/* if END key is hit, terminate the loop */
			if ( flag == END )
				break ;
		}

		/* get all sets of y axis entries okayed */
		if ( flag != END )
		{
			for ( i = 0 ; i < max_y_sets ; i++ )
			{
				for ( entry = 0 ; entry < max_x_entries ; entry++ )
				{
					getnum ( &flag, &yrange[entry][i], entry, col[i], END ) ;

					/* if End key is hit, break out of the loop */
					if ( flag == END )
						break ;
				}

				if ( flag == END )
					break ;
			}
		}

		/* if verification of data is over, terminate the loop */
		if ( flag == END )
			break ;
	}

	/* erase the last row, display message, receive user's response */
	cleartext() ;
	writestring ( "Do you want to save the data in a file (Y/N) ?", 23, 12, 32 ) ;
	fflush ( stdin ) ;
	ans = getch() ;

	if ( ans == 'Y' || ans == 'y' )
	{
		/* calculate area required to save screen contents where box is to be popped up */
		areareqd = ( 16 - 9 + 1 ) * ( 75 - 3 + 1 ) * 2 ;

		/* allocate memory, if unsuccessful display error and terminate execution */
		p = malloc ( areareqd ) ;
		if ( p == NULL )
			alloc_error_exit() ;

		savevideo ( 9, 3, 16, 75, p ) ;  /* save screen contents */
		menubox ( 9, 3, 16, 75, 127, 07 ) ;  /* display a filled box */

		/* if data and legend or only data have been entered from keyboard (i.e. not saved to a file so far) */
		if ( data_type == 3 || data_type == 4 )
		{
			/* ask for file name */
			writestring ( "Enter name of the file:", 10, 5, 127 ) ;

			size ( 5, 7 ) ;  /* display cursor */

			/* collect the file name entered */
			getstring ( filename, 10, 31, 127 ) ;

			size ( 32, 0 ) ;  /* hide cursor */

			fp = fopen ( filename, "w" ) ;

			/* if unsuccessful in opening file */
			if ( fp == NULL )
			{
				writestring ( "Error in opening file!", 12, 5, 127 ) ;
				writestring ( "Press any key...", 14, 5, 127 ) ;
				fflush ( stdin ) ;
				getch() ;

				restorevideo ( 9, 3, 16, 75, p ) ;
				free ( p ) ;
				return ;
			}
		}

		/* if data and legend or only data have been read from the file */
		if ( data_type == 1 || data_type == 2 )
		{
			writestring ( "Overwriting existing data...", 10, 5, 127 ) ;

			fp = fopen ( filename, "r+" ) ;

			/* if unsuccessful in opening file */
			if ( fp == NULL )
			{
				writestring ( "Error in opening file!", 12, 5, 127 ) ;
				writestring ( "Press any key...", 14, 5, 127 ) ;
				fflush ( stdin ) ;
				getch() ;

				restorevideo ( 9, 3, 16, 75, p ) ;
				free ( p ) ;
				return ;
			}
		}

		writestring ( "Please wait...", 12, 5, 127 ) ;

		/* store the data in the file */
		storedata ( fp ) ;

		fclose ( fp ) ;

		/* if legend has also been entered, append it after data */
		if ( data_type == 3 )
		{
			storelegend ( fp ) ;

			/* set data_type to indicate that data and legend are saved */
			data_type = 2 ;
		}

		if ( data_type == 4 )
			data_type = 1 ;  /* set data_type to indicate that data is saved */

		writestring ( "Data values successfully stored!", 12, 5, 127 ) ;
		writestring ( "Press any key...", 14, 5, 127 ) ;
		fflush ( stdin ) ;
		getch() ;

		/* restore original screen contents */
		restorevideo ( 9, 3, 16, 75, p ) ;

		/* free allocated memory */
		free ( p ) ;
	}
}

/* accepts a string from the keyboard */
getstring ( char *str, int row, int col, int color )
{
	int ch, i = 0 ;

	str[0] = '\0' ;
	size ( 5, 7 ) ;

	while ( 1 )
	{
		gotoxy ( col + i + 1, row + 1 ) ;

		fflush ( stdin ) ;
		ch = getch() ;

		if ( ch == '\r' )
			break ;

		if ( ch == '\b' )
		{
			if ( i != 0 )
			{
				i-- ;
				writechar ( row, col + i, ' ', color ) ;
			}
		}

		/* display the character only if it is an ascii character and not a control character */
		if ( isascii ( ch ) && !iscntrl ( ch ) )
		{
			str[i] = ch ;
			writechar ( row, col + i, ch, color ) ;
			i++ ;

			/* if the string entered exceeds 30 characters */
			if ( i > 29 )
				break ;
		}
	}

	str[i] = '\0' ;  /* terminate the string */
	size ( 32, 0 ) ;  /* hide cursor */
}

/* saves the data in a file in a predetermined format */
storedata ( FILE *fp )
{
	int i, j ;

	fprintf ( fp, "%d\n", max_x_entries ) ;
	fprintf ( fp, "%d\n", max_y_sets ) ;

	/* store x axis entries */
	for ( i = 0 ; i < max_x_entries ; i++ )
		fprintf ( fp, "%lf\n", xrange[i] ) ;

	/* store y axis entries */
	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		for ( j = 0 ; j < max_y_sets ; j++ )
			fprintf ( fp, "%lf\n", yrange[i][j] ) ;
	}
}

/* allows changes in current data */
editdata()
{
	int areareqd ;
	char *p ;

	/* calculate area required to save entire screen contents */
	areareqd = ( 24 - 0 + 1 ) * ( 79 - 0 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	savevideo ( 0, 0, 24, 79, p ) ;

	/* display current data */
	displaydata() ;

	/* if any data exists, get it okayed */
	if ( max_x_entries > 0 )
		verifydata() ;

	restorevideo ( 0, 0, 24, 79, p ) ;
	free ( p ) ;
}

/* displays current data on screen */
displaydata()
{
	int col[7] = { 13, 24, 35, 46, 57, 68, 2 }, i, j ;
	char str[12] ;

	/* draw the box used for displaying the data */
	draw_entrybox() ;

	/* if no entries have been supplied so far */
	if ( max_x_entries < 1 )
	{
		writestring ( "Oops! No data. Go back to Data menu.", 23, 1, 47 ) ;
		writestring ( "Press any key...", 23, 62, 47 ) ;
		fflush ( stdin ) ;
		getch() ;

		return ;
	}

	/* display all the x axis entries */
	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		/* convert floating point number to string and display it */
		gcvt ( xrange[i], 10, str ) ;
		writestring ( str, i + 6, col[6], 47 ) ;
	}

	/* display all sets of y axis entries */
	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		for ( j = 0 ; j < max_y_sets ; j++ )
		{
			gcvt ( yrange[i][j], 10, str ) ;
			writestring ( str, i + 6, col[j], 47 ) ;
		}
	}
}

/* accepts legend entries */
legendentry()
{
	int row, i, areareqd ;
	char *p, ans ;
	FILE *fp ;

	/* calculate area required to save entire screen contents */
	areareqd = ( 24 - 0 + 1 ) * ( 79 - 0 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	/* save screen contents */
	savevideo ( 0, 0, 24, 79, p ) ;

	/* draw a filled box */
	menubox ( 0, 0, 24, 79, 47, 0 ) ;

	/* draw a double lined box */
	drawbox ( 0, 0, 24, 79, 47 ) ;

	/* draw a horizontal line */
	for ( i = 1 ; i <= 78 ; i++ )
		writechar ( 22, i, 205, 47 ) ;

	writechar ( 22, 0, 204, 47 ) ;
	writechar ( 22, 79, 185, 47 ) ;

	writestring ( "  Legend Entry  ", 0, 32, 48 ) ;

	/* if no data has been entered so far */
	if ( max_x_entries < 1 )
	{
		writestring ( "Oops! No data. Go back to Data menu.", 23, 1, 32 ) ;
		writestring ( "Press any key...", 23, 62, 32 ) ;
		fflush ( stdin ) ;
		getch() ;

		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;
		return ;
	}

	writestring ( "Graph Title   :", 2, 10, 47 ) ;
	writestring ( "Enter the title of the graph", 23, 2, 32 ) ;
	fflush ( stdin ) ;
	getstring ( titles[0], 2, 27, 32 ) ;

	writestring ( "X-axis Title  :", 4, 10, 47 ) ;
	writestring ( "Enter the title of X - axis ", 23, 2, 32 ) ;
	fflush ( stdin ) ;
	getstring ( titles[1], 4, 27, 32 ) ;

	writestring ( "Y-axis Title  :", 6, 10, 47 ) ;
	writestring ( "Enter the title of Y - axis ", 23, 2, 32 ) ;
	fflush ( stdin ) ;
	getstring ( titles[2], 6, 27, 32 ) ;

	writestring ( "-----: LEGEND :-----", 8, 10, 32 ) ;
	writestring ( "Entry for legend            ", 23, 2, 32 ) ;
	row = 10 ;

	/* receive legends for different sets of y axis values */
	for ( i = 0 ; i < max_y_sets ; i++ )
	{
		writechar ( row, 15, arr[i], 47 ) ;
		writechar ( row, 17, ':', 47 ) ;
		fflush ( stdin ) ;
		getstring ( y_sets_desc[i], row, 19, 32 ) ;
		row += 2 ;
	}

	size ( 32, 0 ) ;

	/* if data is entered from keyboard */
	if ( data_type == 4 )
	{
		/* set up data_type to indicate that data and legend have been entered  from keyboard */
		data_type = 3 ;
	}

	/* erase last row, display message, receive user's choice */
	cleartext() ;
	writestring ( "Do you want to save the data in a file (Y/N) ?", 23, 12, 32 ) ;
	fflush ( stdin ) ;
	ans = getch() ;

	if ( ans == 'Y' || ans == 'y' )
	{
		/* restore original screen contents and free allocated memory */
		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;

		/* calculate area required to save screen contents where box is to be popped up */
		areareqd = ( 16 - 9 + 1 ) * ( 75 - 3 + 1 ) * 2 ;

		/* allocate memory, if unsuccessful display error and terminate execution */
		p = malloc ( areareqd ) ;
		if ( p == NULL )
			alloc_error_exit() ;

		savevideo ( 9, 3, 16, 75, p ) ;
		menubox ( 9, 3, 16, 75, 127, 07 ) ;

		/* if data and legend have not been saved */
		if ( data_type == 3 )
		{
			writestring ( "Enter name of the file:", 10, 5, 127 ) ;
			size ( 5, 7 ) ;
			getstring ( filename, 10, 31, 127 ) ;
			size ( 32, 0 ) ;
		}

		/* if data and legend have already been saved */
		if ( data_type == 2 )
		{
			writestring ( "Legend already saved! Overwrite (Y/N) ?", 10, 5, 127 ) ;
			fflush ( stdin ) ;
			ans = getch() ;
			writechar ( 10, 47, ans, 127 ) ;
		}

		if ( ans == 'y' || ans == 'Y' || data_type == 3 )
		{
			fp = fopen ( filename, "w" ) ;

			/* if unsuccessful in opening file */
			if ( fp == NULL )
			{
				writestring ( "Error in opening file!", 12, 5, 127 ) ;
				writestring ( "Press any key...", 14, 5, 127 ) ;
				fflush ( stdin ) ;
				getch() ;

				restorevideo ( 9, 3, 16, 75, p ) ;
				free ( p ) ;
				return ;
			}

			writestring ( "Please wait...", 12, 5, 127 ) ;

			/* store data in the file */
			storedata ( fp ) ;

			/* add the legend entries at the end of the file */
			storelegend ( fp ) ;

			fclose ( fp ) ;

			/* set up data_type to indicate that data and legend have been saved */
			data_type = 2 ;

			restorevideo ( 9, 3, 16, 75, p ) ;
			free ( p ) ;
		}

		if ( data_type == 1 )
		{
			writestring ( "Appending to file containing data...", 10, 5, 127 ) ;
			fp = fopen ( filename, "a" ) ;

			/* if unable to open file */
			if ( fp == NULL )
			{
				writestring ( "Error in opening file!", 12, 5, 127 ) ;
				writestring ( "Press any key...", 14, 5, 127 ) ;
				fflush ( stdin ) ;
				getch() ;

				restorevideo ( 9, 3, 16, 75, p ) ;
				free ( p ) ;
				return ;
			}

			writestring ( "Please wait...", 12, 5, 127 ) ;

			/* add legend entries at the end of data values */
			storelegend ( fp ) ;

			fclose ( fp ) ;

			/* set up data_type to indicate that data and legend have been saved */
			data_type = 2 ;

			restorevideo ( 9, 3, 16, 75, p ) ;
			free ( p ) ;
		}
	}
	else
	{
		/* control reaches here if user doesn't wish to save the legend entries */

		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;
	}
}

/* saves the current legend entries into a file in a predetermined format */
storelegend ( FILE *fp )
{
	int i ;

	/* take file pointer to the end */
	fseek ( fp, 0, SEEK_END ) ;

	for ( i = 0 ; i <= 2 ; i++ )
		fprintf ( fp, "%s\n", titles[i] ) ;

	for ( i = 0 ; i < max_y_sets ; i++ )
		fprintf ( fp, "%s\n", y_sets_desc[i] ) ;

	writestring ( "Data values & Legend entries successfully stored!", 12, 5, 127 ) ;
	writestring ( "Press any key...", 14, 5, 127 ) ;
	fflush ( stdin ) ;
	getch() ;
}

/* reads data stored in a file */
data_from_file()
{
	int i, j, areareqd, len ;
	FILE *fp ;
	char *p ;

	/* calculate area required to save screen contents where box is to be popped up */
	areareqd = ( 16 - 9 + 1 ) * ( 75 - 3 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	savevideo ( 9, 3, 16, 75, p ) ;
	menubox ( 9, 3, 16, 75, 127, 07 ) ;

	writestring ( "Enter name of the file:", 10, 5, 127 ) ;
	size ( 5, 7 ) ;
	getstring ( filename, 10, 31, 127 ) ;
	size ( 32, 0 ) ;

	fp = fopen ( filename, "r" ) ;

	/* if unsuccessful in opening file */
	if ( fp == NULL )
	{
		writestring ( "Error in opening file!", 12, 5, 127 ) ;
		writestring ( "Press any key...", 14, 5, 127 ) ;
		fflush ( stdin ) ;
		getch() ;

		restorevideo ( 9, 3, 16, 75, p ) ;
		free ( p ) ;
		return ;
	}

	writestring ( "Please wait...", 12, 5, 127 ) ;

	data_type = 1 ;  /* set up data_type to indicate that data is read from file */

	/* read maximum number of x entries and maximum sets of y entries */
	fscanf ( fp, "%d", &max_x_entries ) ;
	fscanf ( fp, "%d", &max_y_sets ) ;

	/* read x entries from the file */
	for ( i = 0 ; i < max_x_entries ; i++ )
		fscanf ( fp, "%lf", &xrange[i] ) ;

	/* read y entries from the file */
	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		for ( j = 0 ; j < max_y_sets ; j++ )
			fscanf ( fp, "%lf", &yrange[i][j] ) ;
	}

	/* bypass the newline character separating the data values from the legend entries */
	fgetc ( fp ) ;

	/* check whether legend entries are present in file */
	if ( fgets ( titles[0], 80, fp ) == NULL )
		writestring ( "Legend not set...", 12, 5, 127 ) ;
	else
	{
		/* set up data_type to indicate that data and legend have been read from the file */
		data_type = 2 ;

		/* terminate string */
		len = strlen ( titles[0] ) ;
		titles[0][len - 1] = '\0' ;

		/* read legend entries */
		for ( i = 1 ; i <= 2 ; i++ )
		{
			fgets ( titles[i], 80, fp ) ;
			len = strlen ( titles[i] ) ;
			titles[i][len - 1] = '\0' ;
		}

		for ( i = 0 ; i < max_y_sets ; i++ )
		{
			fgets ( y_sets_desc[i], 80, fp ) ;
			len = strlen ( y_sets_desc[i] ) ;
			y_sets_desc[i][len - 1] = '\0' ;
		}
	}

	writestring ( "Values successfully set up!", 13, 5, 127 ) ;
	writestring ( "Press any key...",14, 5, 127 ) ;
	fflush ( stdin ) ;
	getch() ;

	fclose ( fp ) ;
	restorevideo ( 9, 3, 16, 75, p ) ;
	free ( p ) ;
}

/* draws a bar chart */
bar_chart()
{
	char *p, str[13] ;
	double big, small, scale_factor, incr_val, total ;
	int areareqd, xright, xleft, ytop, ybottom, total_bars, endx, posi_of_xaxis ;
	int i, j, xincr, bar_width, y_incr, boxheight, graphwidth, graphheight ;

	/* calculate area required to save screen contents where chart is to be drawn */
	areareqd = ( 24 - 0 + 1 ) * ( 79 - 0 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	/* save screen contents into allocated memory */
	savevideo ( 0, 0, 24, 79, p ) ;

	/* if data doesn't exist */
	if ( max_x_entries < 1 )
	{
		menubox ( 11, 8, 16, 71, 127, 07 ) ;
		writestring ( "Oops! No data. Go back to Data menu.", 12, 9, 127 ) ;
		writestring ( "Press any key...", 14, 9, 127 ) ;
		fflush ( stdin ) ;
		getch() ;

		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;
		return ;
	}

	/* the chart is drawn within a box with dimensions 528 x 136 */
	/* corner coordinates of the box are (104, 24) and (631, 163) */
	boxheight = 163 - 24 + 1 ;
	graphwidth = 528 ;

	/* find the biggest and smallest y axis entry */
	small = big = yrange[0][0] ;
	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		for ( j = 0 ; j < max_y_sets ; j++ )
		{
			if ( big < yrange[i][j] )
				big = yrange[i][j] ;

			if ( small > yrange[i][j] )
				small = yrange[i][j] ;
		}
	}

	/* if all data values are positive */
	if ( small >= 0 )
	{
		total = big ;
		graphheight = boxheight - 10 ;

		/* calculate the position where x axis is to be placed */
		posi_of_xaxis = 163 ;
	}
	else
	{
		/* if all data values are negative */
		if ( big < 0 )
		{
			big = 0 ;
			total = big - small ;
			graphheight = boxheight - 20 ;

			/* calculate the position where x axis is to be placed */
			posi_of_xaxis = 22 + 10 ;
		}
		else
		{
			/* some data values are positive and some are negative */
			total = big - small ;
			graphheight = boxheight - 20 ;

			/* calculate the position where x axis is to be placed */
			posi_of_xaxis = 163 - 10 - graphheight / ( ( big / -small ) + 1 ) ;
		}
	}

	/* calculate the y axis scaling factor */
	scale_factor = total / graphheight ;

	/* calculate the total number of bar widths required */
	total_bars = max_x_entries * max_y_sets + ( max_x_entries - 1 ) ;

	/* calculate the width of each bar */
	bar_width = graphwidth / total_bars ;

	/* calculate the exact x coordinate of right corner of the box */
	endx = bar_width * total_bars + 104 ;

	/* change to graphics mode */
	graphicsmode() ;

	/* draw the box */
	rectangle ( 104, 22, endx, 163 ) ;

	/* draw the x axis */
	line ( 104, posi_of_xaxis, endx, posi_of_xaxis ) ;

	/* set the variables for drawing the first bar */
	xleft = 104 ;
	xright = 104 + bar_width ;

	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		for ( j = 0 ; j < max_y_sets ; j++ )
		{
			/* if the data value is negative */
			if ( yrange[i][j] >= 0 )
			{
				/* set the y coordinate from which the bar should begin */
				ybottom = posi_of_xaxis ;

				/* calculate the height of the bar */
				ytop = ybottom - yrange[i][j] / scale_factor ;
			}
			else
			{
				/* set the y coordinate from which the bar should begin */
				ytop = posi_of_xaxis ;

				/* calculate the height of the bar */
				ybottom = ytop - yrange[i][j] / scale_factor ;
			}

			/* set up the fill style for the bar to be drawn */
			setfillstyle ( 1 + j, WHITE ) ;

			/* draw a bar in the current fill style */
			bar3d ( xleft, ytop, xright, ybottom, 0, 0 ) ;

			/* update the variables for the next bar */
			xright += bar_width ;
			xleft += bar_width ;
		}

		/* leave space (equal to one bar width) between two consecutive clusters of bars */
		xright += bar_width ;
		xleft += bar_width ;
	}

	/* set up text justification */
	settextjustify ( CENTER_TEXT, TOP_TEXT ) ;

	/* calculate the number of pixels for each division along the y axis */
	y_incr = graphheight / 10 ;

	/* calculate the magnitude of each division */
	incr_val = total / 10 ;

	/* if all data values are positive */
	if ( small >= 0 )
	{
		for ( i = 0 ; i <= 10 ; i++ )
		{
			/* convert the floating point magnitude to a string */
			gcvt ( incr_val * i , 10, str ) ;

			/* display the string along the y axis */
			moveto ( 52, ( 163 - 4 - ( y_incr * i ) ) ) ;
			outtext ( str ) ;

			/* mark the corresponding y axis division */
			line ( 99, 163 - ( y_incr * i ), 99 + 4, 163 - ( y_incr * i ) ) ;
		}
	}
	else
	{
		/* mark the divisions along the positive direction of y axis */
		i = 0 ;
		while ( posi_of_xaxis - ( y_incr * i ) >= 22 )
		{
			/* convert the floating point magnitude to a string */
			gcvt ( incr_val * i , 10, str ) ;

			/* display the string along the y axis */
			moveto ( 52, ( posi_of_xaxis - 4 - ( y_incr * i ) ) ) ;
			outtext ( str ) ;

			line ( 99, posi_of_xaxis - ( y_incr * i ), 99 + 4, posi_of_xaxis - ( y_incr * i ) ) ;
			i++ ;
		}

		/* mark the divisions along the nàÇ àOâÇàÇ  œÄœÇOéÇO¿¿ppÚrO¿O∆O¿ppÚr ÇàO√œáÄ  ÄéÇO¿O√œÇ  åœ¿œÇO¬O¡¿O∆@ƒO¬ppÚr ppÚtÅ  g∆  @D@Á¿¡ƒ'ƒ@¿ ¡ D'√¿ @Á¿ A ¿ƒ'¿¿'«'¬ ¬A D'‡`88y99 @Á·Á‡'·Á¡ F@ G Á‡g¡'·'‡Á„#‡Á¬ ¬g·H√BÉBÇBBK à\‹^HH›ö[ô»[€ô»HH^\»
ã√BÇBB[[›ô]»
Lã
‹⁄W€Ÿóﬁ^\»H
»
W⁄[ò‹à
àH
H
H
H√BÇBB[›]^
›à
H√BÉBÇBB[[ôH
NK‹⁄W€Ÿóﬁ^\»
»
W⁄[ò‹à
àH
KNH
»‹⁄W€Ÿóﬁ^\»
»
W⁄[ò‹à
àH
H
H√BÇBBZJ »√BÇB_CBÇ_CBÉBÇK àÿ[›[]HHù[Xô\àŸà^[»õ‹àXX⁄]ö\⁄[€à[€ô»Hx axis */
	xincr = 104 + ( bar_width * max_y_sets ) / 2 ;

	/* display the x axis entries */
	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		/* convert the floating point x axis entry to a string */
		gcvt ( xrange[i], 10, str ) ;

		/* write the string */
		moveto ( xincr, 168 ) ;
		outtext ( str ) ;

		/* mark the corresponding x axis division */
		line ( xincr, 163, xincr, 163 + 4 ) ;

		/* go to the next division along x axis */
		xincr += ( bar_width * ( max_y_sets + 1 ) ) ;
	}

	/* if legend entries exist, display them */
	if ( data_type == 2 || data_type == 3 )
		displaylegend ( 1 ) ;

	/* store the current graph in allocated memory */
	getimage ( 0, 0, 639, 199, graph_ptr ) ;

	/* wait till a key is hit */
	fflush ( stdin ) ;
	getch() ;

	/* return to text mode */
	closegraph() ;
	restorecrtmode() ;
	size ( 32, 0 ) ;
	restorevideo ( 0, 0, 24, 79, p ) ;
	free ( p ) ;
}

/* displays legend entries */
displaylegend ( int graph_type )
{
	int xincr, i, graphwidth = 528 ;

	/* display the title of the graph */
	moveto ( 368, 8 ) ;
	settextjustify ( CENTER_TEXT, TOP_TEXT ) ;
	settextstyle ( DEFAULT_FONT, HORIZ_DIR, 1 ) ;
	outtext ( titles[0] ) ;

	/* display the label given to the x axis */
	moveto ( 368, 178 ) ;
	outtext ( titles[1] ) ;

	settextjustify ( LEFT_TEXT, TOP_TEXT ) ;
	xincr = graphwidth / max_y_sets ;

	/* if a bar chart or a stacked bar chart is being drawn */
	if ( graph_type == 1 || graph_type == 2 )
	{
		/* display the description of each fill pattern */
		for ( i = 0 ; i < max_y_sets ; i++ )
		{
			setfillstyle ( 1 + i, WHITE ) ;
			bar3d ( 104 + i * xincr, 190, 104 + i * xincr + 8, 198, 0, 0 ) ;
			outtextxy ( 104 + i * xincr + 15, 190, y_sets_desc[i] ) ;
		}
	}
	else
	{
		/* if an XY chart or a line chart is being drawn */
		for ( i = 0 ; i < max_y_sets ; i++ )
		{
			/* display the description of each symbol */
			outtextxy ( 104 + i * xincr, 190, symbol[i] ) ;
			outtextxy ( 104 + i * xincr + 15, 190, y_sets_desc[i] ) ;
		}
	}

	/* display the label for the y axis */
	moveto ( 8, 84 ) ;
	settextjustify ( RIGHT_TEXT, CENTER_TEXT ) ;
	settextstyle ( DEFAULT_FONT, VERT_DIR, 1 ) ;
	outtext ( titles[2] ) ;
}

/* draws a stacked bar chart */
stack_bar_chart()
{
	char *p, str[13] ;
	double big, scale_factor, positivesum, negativesum, incr_val, small, total ;
	int i, j, areareqd, xright, xleft, neg_ytop, neg_ybottom, pos_ytop, pos_ybottom, endx, posi_of_xaxis ;
	int xincr, bar_width, y_incr, graphheight, graphwidth, boxheight ;

	/* calculate area required to save screen contents where chart is to be drawn */
	areareqd = ( 24 - 0 + 1 ) * ( 79 - 0 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	/* save screen contents into allocated memory */
	savevideo ( 0, 0, 24, 79, p ) ;

	/* if data doesn't exist */
	if ( max_x_entries < 1 )
	{
		menubox ( 11, 8, 16, 71, 127, 07 ) ;
		writestring ( "Oops! No data. Go back to Data menu.", 12, 9, 127 ) ;
		writestring ( "Press any key...", 14, 9, 127 ) ;
		fflush ( stdin ) ;
		getch() ;

		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;
		return ;
	}

	/* the chart is drawn within a box with dimensions 528 x 136 */
	/* corner coordinates of the box are (104, 24) and (631, 163) */
	boxheight = 163 - 24 + 1 ;
	graphwidth = 528 ;

	small = 0 ;
	big = 0 ;

	/* calculate the largest positive sum and largest negative sum of all y axis entries */
	for	( i = 0 ; i < max_x_entries ; i++ )
	{
		/* find the positive sum and the negative sum of all y axis entries for a given x axis entry */
		positivesum = 0 ;
		negativesum = 0 ;
		for ( j = 0 ; j < max_y_sets ; j++ )
		{
			if ( yrange[i][j] > 0 )
				positivesum += yrange[i][j] ;
			else
				negativesum += yrange[i][j] ;
		}

		if ( big < positivesum )
			big = positivesum ;

		if ( small > negativesum )
			small = negativesum ;
	}

	/* if all data values are positive */
	if ( small >= 0 )
	{
		total = big ;
		graphheight = boxheight - 10 ;

		/* calculate the position where x axis is to be placed */
		posi_of_xaxis = 163 ;
	}
	else
	{
		total = big - small ;
		graphheight = boxheight - 20 ;

		if ( big == 0 )
		{
			/* if all data values are negative */
			posi_of_xaxis = 22 + 10 ;
		}
		else
		{
			/* some data values are positive and some are negative */
			posi_of_xaxis = 163 - 10 - graphheight / ( ( big / -small ) + 1 ) ;
		}
	}

	/* calculate the y axis scaling factor */
	scale_factor = total / graphheight ;

	/* calculate the width of each bar */
	bar_width = ( graphwidth - 20 * ( max_x_entries - 1 ) ) / max_x_entries ;

	/* initialise the variable xincr such that 20 pixels are left between adjacent bars */
	xincr = bar_width + 20 ;

	/* calculate the exact x-coordinate of right corner of the box */
	endx = bar_width * max_x_entries + 20 * ( max_x_entries - 1 ) + 104 ;

	/* change to graphics mode */
	graphicsmode() ;

	/* draw the box */
	rectangle ( 104, 24, endx, 163 ) ;

	/* draw the x axis */
	line ( 104, posi_of_xaxis, endx, posi_of_xaxis ) ;

	/* set the variables for drawing the first bar */
	xleft = 104 ;
	xright = 104 + bar_width ;

	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		/* set the y coordinate from which the stacked bar should begin */
		neg_ytop = posi_of_xaxis ;
		pos_ybottom = posi_of_xaxis ;

		for ( j = 0 ; j < max_y_sets ; j++ )
		{
			if ( yrange[i][j] >= 0 )
			{
				/* calculate the height of each bar */
				pos_ytop = pos_ybottom - yrange[i][j] / scale_factor ;

				/* draw a bar in the current fill pattern */
				setfillstyle ( 1 + j, WHITE ) ;
				bar3d ( xleft, pos_ytop, xright, pos_ybottom, 0, 0 ) ;

				/* set the base of the next bar to the top of the previous one */
				pos_ybottom = pos_ytop ;
			}
			else
			{
				/* calculate the height of the bar */
				neg_ybottom = neg_ytop - yrange[i][j] / scale_factor ;

				/* draw a bar in the current fill pattern */
				setfillstyle ( 1 + j, WHITE ) ;
				bar3d ( xleft, neg_ytop, xright, neg_ybottom, 0, 0 ) ;

				/* set the top of next bar to the bottom of previous one */
				neg_ytop = neg_ybottom ;
			}
		}

		/* update the variables for the next bar */
		xright += xincr ;
		xleft += xincr ;
	}

	/* set up text justification */
	settextjustify ( CENTER_TEXT, TOP_TEXT ) ;

	/* calculate the number of pixels for each division along the y axis */
	y_incr = graphheight / 10 ;

	/* calculate the appropriate magnitude of each division */
	incr_val = total / 10 ;

	/* if all data values are positive */
	if ( small >= 0 )
	{
		for ( i = 0 ; i <= 10 ; i++ )
		{
			/* convert the floating point magnitude to a string */
			gcvt ( incr_val * i, 10, str ) ;
			moveto ( 56, ( 163 - 4 - ( y_incr * i ) ) ) ;

			/* display the string along the y axis */
			outtext ( str ) ;

			/* mark the corresponding y axis division */
			line ( 99, 163 - ( y_incr * i ), 99 + 4, 163 - ( y_incr * i ) ) ;
		}
	}
	else
	{
		/* mark the divisions along the positive direction of y axis */
		i = 0 ;
		while ( posi_of_xaxis - ( y_incr * i ) >= 22 )
		{
			/* convert the floating point magnitude to a string */
			gcvt ( incr_val * i , 10, str ) ;

			/* display the string along the y axis */
			moveto ( 52, ( posi_of_xaxis - 4 - ( y_incr * i ) ) ) ;
			outtext ( str ) ;

			/* mark the corresponding y axis division */
			line ( 99, posi_of_xaxis - ( y_incr * i ), 99 + 4, posi_of_xaxis - ( y_incr * i ) ) ;
			i++ ;
		}

		/* mark the divisions along the negative direction of y axis */
		i = 1 ;
		while ( posi_of_xaxis + ( y_incr * i ) <= 163 )
		{
			/* convert the floating point magnitude to a string */
			gcvt ( - ( incr_val * i ) , 10, str ) ;

			/* display the string along the y axis */
			moveto ( 52, ( posi_of_xaxis - 4 + ( y_incr * i ) ) ) ;
			outtext ( str ) ;

			/* mark the corresponding y axis division */
			line ( 99, posi_of_xaxis + ( y_incr * i ), 99 + 4, posi_of_xaxis + ( y_incr * i ) ) ;
			i++ ;
		}
	}

	/* calculate the number of pixels for each division along the x axis */
	xincr = 104 + bar_width / 2 ;

	settextjustify ( CENTER_TEXT, TOP_TEXT ) ;

	/* display the x axis entries */
	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		gcvt ( xrange[i], 10, str ) ;
		moveto ( xincr, 168 ) ;
		outtext ( str ) ;

		/* mark the corresponding x axis division */
		line ( xincr, 163, xincr, 163 + 4 ) ;

		xincr += ( bar_width + 20 ) ;
	}

	/* if legend entries exist, display them */
	if ( data_type == 2 || data_type == 3 )
		displaylegend ( 2 ) ;

	/* store the current graph in allocated memory */
	getimage ( 0, 0, 639, 199, graph_ptr ) ;

	fflush ( stdin ) ;
	getch() ;

	/* return to text mode */
	closegraph() ;
	restorecrtmode() ;

	size ( 32, 0 ) ;
	restorevideo ( 0, 0, 24, 79, p ) ;
	free ( p ) ;
}

/* draws a pie chart */
pie_chart()
{
	char *p, str[12] ;
	double sum = 0, percent[15] ;
	int i, j, areareqd, xc, yc, start_angle, end_angle ;

	/* calculate area required to save screen contents where chart is to be drawn */
	areareqd = ( 24 - 0 + 1 ) * ( 79 - 0 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	/* save screen contents into allocated memory */
	savevideo ( 0, 0, 24, 79, p ) ;

	/* if data doesn't exist */
	if ( max_x_entries < 1 )
	{
		menubox ( 11, 8, 16, 71, 127, 07 ) ;
		writestring ( "Oops! No data. Go back to Data menu.", 12, 9, 127 ) ;
		writestring ( "Press any key...", 14, 9, 127 ) ;
		fflush ( stdin ) ;
		getch() ;

		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;
		return ;
	}

	/* change to graphics mode */
	graphicsmode() ;

	/* draw as many pie charts as there are x entries */
	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		/* calculate sum of all y axis entries for each x axis entry */
		sum = 0 ;
		for ( j = 0 ; j < max_y_sets ; j++ )
			sum += fabs ( yrange[i][j] ) ;

		/* set the variable for drawing the first sector of the chart */
		start_angle = 0 ;

		for ( j = 0 ; j < max_y_sets ; j++ )
		{
			/* determine the magnitude of the angle of each sector */
			end_angle = ceil ( ( fabs ( yrange[i][j] ) * 360.0 ) / sum ) ;

			/* calculate the percentage of the chart occupied by each sector */
			percent[j] = 100.0 * end_angle / 360 ;

			/* determine the angle at which each sector should end */
			end_angle += start_angle ;

			/* if last sector, round off ending angle to 360 */
			if ( j == max_y_sets - 1 )
				end_angle = 360 ;

			if ( percent[j] != 0 )
			{
				/* draw a sector in the current fill style */
				setfillstyle ( 1 + j, WHITE ) ;
				sector ( 319, 99, start_angle, end_angle, 110, 60 ) ;
			}
			else
				pieslice ( 319, 99, start_angle, end_angle, 110 ) ;

			/* set the start of next sector at the end of the previous one */
			start_angle = end_angle ;
		}

		/* display the legend entries */

		xc = 10 ;
		yc = 20 ;

		/* set up text justification */
		settextjustify ( LEFT_TEXT, TOP_TEXT ) ;

		for ( j = 0 ; j < max_y_sets ; j++ )
		{
			/* draw a sector depicting fill style used */
			setfillstyle ( 1 + j, WHITE ) ;
			sector ( xc, yc, 0, 60, 20, 20 ) ;

			/* if legend entries exist, display them */
			if ( data_type == 2 || data_type == 3 )
			{
				/* display the description of each fill style */
				moveto ( xc + 30, yc - 8 ) ;
				outtext ( y_sets_desc[j] ) ;
			}

			/* display the percentage of chart occupied by each y axis entry */
			gcvt ( percent[j], 10, str ) ;
			moveto ( xc, yc + 8 ) ;
			outtext ( str ) ;
			outtext ( " % " ) ;

			/* update the coordinates for the next sector */
			yc += 50 ;
			if ( j == 2 )
			{
				xc = 550 ;
				yc = 20 ;
			}
		}

		/* if legend entries exist, display them */
		if ( data_type == 2 || data_type == 3 )
		{
			/* display the title of the graph */
			moveto ( 319, 8 ) ;
			settextjustify ( CENTER_TEXT, TOP_TEXT ) ;
			settextstyle ( DEFAULT_FONT, HORIZ_DIR, 1 ) ;
			outtext ( titles[0] ) ;

			/* display the label given to the x axis */
			moveto ( 319, 170 ) ;
			outtext ( titles[1] ) ;

			/* display the x axis entry */
			moveto ( 319, 180 ) ;
			gcvt ( xrange[i], 10, str ) ;
			outtext ( str ) ;
		}

		/* store the current graph in allocated memory */
		getimage ( 0, 0, 639, 199, graph_ptr ) ;

		gotoxy ( 1, 25 ) ;
		printf ( "Press any key for Pie chart of next X range value... Esc to return" ) ;
		fflush ( stdin ) ;
		if ( getch() == 27 )
			break ;

		clearviewport() ;
	}

	/* return to text mode */
	closegraph() ;
	restorecrtmode();

	size ( 32, 0 ) ;
	restorevideo ( 0, 0, 24, 79, p ) ;
	free ( p ) ;
}

/* draws an XY chart or a line chart depending on the argument passed */
xy_or_line ( int graph_type )
{
	char *p, str[12] ;
	double scale_factor_x, scale_factor_y, small_x, small_y, big_x, big_y, incr_val ;
	int i, j, k, areareqd, x, y, xincr, y_incr, endx, xc[15], yc[15][16], temp, graphheight, graphwidth, boxheight, posi_of_xaxis, posi_of_yaxis ;

	/* calculate area required to save screen contents where chart is to be drawn */
	areareqd = ( 24 - 0 + 1 ) * ( 79 - 0 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	/* save screen contents into allocated memory */
	savevideo ( 0, 0, 24, 79, p ) ;

	/* if data doesn't exist */
	if ( max_x_entries < 1 )
	{
		menubox ( 11, 8, 16, 71, 127, 07 ) ;
		writestring ( "Oops! No data. Go back to Data menu.", 12, 9, 127 ) ;
		writestring ( "Press any key...", 14, 9, 127 ) ;
		fflush ( stdin ) ;
		getch() ;

		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;
		return ( 0 ) ;
	}

	boxheight = 163 - 24 + 1 ;
	graphwidth = 496 ;

	/* find the biggest and the smallest y axis entry */
	big_y = yrange[0][0] ;
	small_y = yrange[0][0] ;
	for	( i = 0 ; i < max_x_entries ; i++ )
	{
		for ( j = 0 ; j < max_y_sets ; j++ )
		{
			if ( big_y < yrange[i][j] )
				big_y = yrange[i][j] ;

			if ( small_y > yrange[i][j] )
				small_y = yrange[i][j] ;
		}
	}

	/* if all y axis values are same */
	if ( big_y == small_y )
	{
		/* if that entry is positive */
		if ( big_y > 0 )
			small_y = 0 ;
		else
			big_y = 0 ;
	}

	/* if all y axis entries are positive */
	if ( small_y >= 0 )
	{
		graphheight = boxheight - 10 ;

		/* calculate the position where x axis is to be placed */
		posi_of_xaxis = 163 ;
	}
	else
	{
		/* if all y axis entries are negative */
		if ( big_y < 0 )
		{
			big_y = 0 ;
			graphheight = boxheight - 20 ;

			/* calculate the position where x axis is to be placed */
			posi_of_xaxis = 22 + 10 ;
		}
		else
		{
			/* control reaches here if some y axis entries are positive and some are negative */

			graphheight = boxheight - 20 ;

			/* calculate the position where x axis is to be placed */
			posi_of_xaxis = 163 - 10 - graphheight / ( ( big_y / -small_y ) + 1 ) ;
		}
	}

	/* calculate the y axis scaling factor */
	scale_factor_y = ( big_y - small_y ) / graphheight ;

	/* find the biggest and the smallest x axis entry */
	big_x = small_x = xrange[0] ;
	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		if ( big_x < xrange[i] )
			big_x = xrange[i] ;

		if ( small_x > xrange[i] )
			small_x = xrange[i] ;
	}

	/* if all x axis values are same */
	if ( big_x == small_x )
	{
		if ( big_x > 0 )
			small_x = 0 ;
		else
			big_x = 0 ;
	}

	/* if all x axis entries are positive */
	if ( small_x >= 0 )
	{
		/* calculate the position where y axis is to be placed */
		posi_of_yaxis = 104 ;
	}
	else
	{
		/* if all x axis entries are negative */
		if ( big_x < 0 )
		{
			big_x = 0 ;

			/* set the position where y axis is to be placed */
			posi_of_yaxis = 104 + graphwidth ;
		}
		else
		{
			/* some x axis entries are positive and some are negative */
			posi_of_yaxis = 104 + graphwidth / ( ( big_x / -small_x ) + 1 ) ;
		}
	}

	/* calculate the x axis scaling factor */
	scale_factor_x = ( big_x - small_x ) / graphwidth ;

	/* calculate the exact x-coordinate of right corner of the box */
	endx = ( big_x - small_x ) / scale_factor_x + 104 ;

	/* change to graphics mode */
	graphicsmode() ;

	/* draw the box */
	rectangle ( 104, 22, endx, 163 ) ;

	/* draw the x axis and the y axis */
	line ( 104, posi_of_xaxis, endx, posi_of_xaxis ) ;
	line ( posi_of_yaxis, 22, posi_of_yaxis, 163 ) ;

	/* plot all the points on the graph */
	for ( i = 0 ; i < max_x_entries ; i++ )
	{
		/* find the coordinate corresponding to each x axis entry */
		if ( small_x > 0 )
			x = posi_of_yaxis + ( xrange[i] - small_x ) / scale_factor_x ;
		else
			x = posi_of_yaxis + xrange[i] / scale_factor_x ;

		xc[i] = x ;

		/* find all the y coordinates for each x axis entry */
		for ( j = 0 ; j < max_y_sets ; j++ )
		{
			/* find the coordinate corresponding to each x axis entry */
			if ( small_y > 0 )
				y = posi_of_xaxis - ( yrange[i][j] - small_y ) / scale_factor_y ;
			else
				y = posi_of_xaxis - yrange[i][j] / scale_factor_y ;

			yc[i][j] = y ;

			/* mark the point using appropriate symbol */
			outtextxy ( x - 4, y - 4, symbol[j] ) ;
		}
	}

	/* calculate the number of pixels for each division along the y axis */
	y_incr = graphheight / 10 ;

	/* calculate the magnitude of each division */
	incr_val = ( big_y - small_y ) / 10 ;

	settextjustify ( CENTER_TEXT, TOP_TEXT ) ;

	/* if all y axis values are positive */
	if ( small_y >= 0 )
	{
		for ( i = 0 ; i <= 10 ; i++ )
		{
			/* convert the floating point magnitude to a string */
			gcvt ( small_y + ( incr_val * i ), 10, str ) ;

			/* display the string along the y axis */
			moveto ( 56, ( 163 - 4 - ( y_incr * i ) ) ) ;
			outtext ( str ) ;

			/* mark the corresponding y axis division */
			line ( 99, 163 - ( y_incr * i ), 99 + 4, 163 - ( y_incr * i ) ) ;
		}
	}
	else
	{
		/* mark the divisions along the positive direction of y axis */
		i = 0 ;
		while ( posi_of_xaxis - ( y_incr * i ) >= 22 )
		{
			/* convert the floating point magnitude to a string */
			gcvt ( incr_val * i , 10, str ) ;

			/* display the string along the y axis */
			moveto ( 52, ( posi_of_xaxis - 4 - ( y_incr * i ) ) ) ;
			outtext ( str ) ;

			line ( 99, posi_of_xaxis - ( y_incr * i ), 99 + 4, posi_of_xaxis - ( y_incr * i ) ) ;
			i++ ;
		}

		/* mark the divisions along the negative direction of y axis */
		i = 1 ;
		while ( posi_of_xaxis + ( y_incr * i ) <= 163 )
		{
			/* convert the floating point magnitude to a string */
			gcvt ( - ( incr_val * i ) , 10, str ) ;

			/* display the string along the y axis */
			moveto ( 52, ( posi_of_xaxis - 4 + ( y_incr * i ) ) ) ;
			outtext ( str ) ;

			line ( 99, posi_of_xaxis + ( y_incr * i ), 99 + 4, posi_of_xaxis + ( y_incr * i ) ) ;
			i++ ;
		}
	}

	if ( small_x >= 0 && max_x_entries > 1 )
	{
		/* calculate the number of pixels for each division along the x axis */
		xincr = ( endx - 104 + 1 ) / ( max_x_entries - 1 ) ;

		/* calculate the magnitude of each division */
		incr_val = ( big_x - small_x ) / ( max_x_entries - 1 ) ;
	}
	else
	{
		/* calculate the number of pixels for each division along the x axis */
		xincr = ( endx - 104 + 1 ) / 8 ;

		/* calculate the magnitude of each division */
		incr_val = ( big_x - small_x ) / 8 ;
	}

	settextjustify ( CENTER_TEXT, TOP_TEXT ) ;

	/* display the x axis entries */
	if ( small_x > 0 )
	{
		for ( i = 0 ; i < max_x_entries ; i++ )
		{
			/* convert the floating point magnitude to a string */
			gcvt ( small_x + ( incr_val * i ), 10, str ) ;

			/* display the string along the x axis */
			moveto ( 104 + xincr * i , 168 ) ;
			outtext ( str ) ;

			/* mark the corresponding x axis division */
			line ( 104 + xincr * i, 163, 104 + xincr * i, 163 + 4 ) ;
		}
	}
	else
	{
		/* mark the divisions along the positive direction of x axis */
		i = 0 ;
		while ( posi_of_yaxis + xincr * i <= endx )
		{
			/* convert the floating point magnitude to a string */
			gcvt ( incr_val * i, 10, str ) ;

			/* display the string along the x axis */
			moveto ( posi_of_yaxis + xincr * i , 168 ) ;
			outtext ( str ) ;

			/* mark the corresponding x axis division */
			line ( posi_of_yaxis + xincr * i, 163, posi_of_yaxis + xincr * i, 163 + 4 ) ;
			i++ ;
		}

		/* mark the divisions along the negative direction of x axis */
		i = 1 ;
		while ( posi_of_yaxis - xincr * i >= 104 )
		{
			/* convert the floating point magnitude to a string */
			gcvt ( - ( incr_val * i ), 10, str ) ;

			/* display the string along the x axis */
			moveto ( posi_of_yaxis - xincr * i , 168 ) ;
			outtext ( str ) ;

			/* mark the corresponding x axis division */
			line ( posi_of_yaxis - xincr * i, 163, posi_of_yaxis - xincr * i, 163 + 4 ) ;
			i++ ;
		}
	}

	/* if a line chart is to be drawn */
	if ( graph_type == LINE )
	{
		/* arrange the x axis entries in ascending order */
		for ( i = 0 ; i < max_x_entries - 1 ; i++ )
		{
			for ( j = i + 1 ; j < max_x_entries ; j++ )
			{
				if ( xc[i] > xc[j] )
				{
					temp = xc[i] ;
					xc[i] = xc[j] ;
					xc[j] = temp ;

					/* swap corresponding y axis entries */
					for ( k = 0 ; k < max_y_sets ; k++ )
					{
						temp = yc[i][k] ;
						yc[i][k] = yc[j][k] ;
						yc[j][k] = temp ;
					}
				}
			}
		}

		/* join the points of each line on the graph */
		for ( i = 0 ; i < max_x_entries - 1 ; i++ )
		{
			for ( j = 0 ; j < max_y_sets ; j++ )
				line ( xc[i], yc[i][j], xc[i + 1], yc[i + 1][j] ) ;
		}
	}

	/* if legend entries exist, display them */
	if ( data_type == 2 || data_type == 3 )
		displaylegend ( 3 ) ;

	/* store the current graph in allocated memory */
	getimage ( 0, 0, 639, 199, graph_ptr ) ;

	fflush ( stdin ) ;
	getch() ;

	/* return to text mode */
	closegraph() ;
	restorecrtmode() ;

	size ( 32, 0 ) ;
	restorevideo ( 0, 0, 24, 79, p ) ;
	free ( p ) ;
}

/* saves the graph present in memory into a file */
save()
{
	char fname[31], *p ;
	int out, areareqd ;

	/* calculate area required to save entire screen contents */
	areareqd = ( 24 - 0 + 1 ) * ( 79 - 0 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	savevideo ( 0, 0, 24, 79, p ) ;
	menubox ( 9, 3, 16, 75, 127, 7 ) ;

	/* if data doesn't exist */
	if ( max_x_entries < 1 )
	{
		writestring ( "Oops! No data. Go back to Data menu.", 10, 5, 127 ) ;
		writestring ( "Press any key...", 12, 5, 127 ) ;
		fflush ( stdin ) ;
		getch() ;

		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;
		return ;
	}

	writestring ( "Enter name of the file:", 10, 5, 127 ) ;
	size ( 5, 7 ) ;
	getstring ( fname, 10, 31, 127 ) ;
	size ( 32, 0 ) ;

	/* open the file in appropriate mode */
	out = open ( fname, O_BINARY | O_CREAT | O_RDWR, S_IWRITE ) ;

	/* if unable to open file */
	if ( out == -1 )
	{
		writestring ( "Error in opening file!", 12, 5, 127 ) ;
		writestring ( "Press any key...", 14, 5, 127 ) ;
		fflush ( stdin ) ;
		getch() ;

		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;
		return ;
	}

	/* calculate the area occupied by the graph in memory */
	area = 4 + ( ( 640 + 7 ) / 8 ) * 200 ;

	/* copy the graph from memory into the specified file */
	write ( out, graph_ptr, area ) ;

	/* close the file */
	close ( out ) ;

	writestring ( "Graph successfully saved!", 12, 5, 127 ) ;
	writestring ( "Press any key...", 14, 5, 127 ) ;
	fflush ( stdin ) ;
	getch() ;

	restorevideo ( 0, 0, 24, 79, p ) ;
	free ( p ) ;
}

/* loads the graph from the file into memory, and displays it */
load()
{
	char fname[31], *p ;
	int in, areareqd ;

	/* calculate area required to save entire screen contents */
	areareqd = ( 24 - 0 + 1 ) * ( 79 - 0 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	savevideo ( 0, 0, 24, 79, p ) ;
	menubox ( 9, 3, 16, 75, 127, 07 ) ;

	writestring ( "Enter name of the file:", 10, 5, 127 ) ;
	size ( 5, 7 ) ;
	getstring ( fname, 10, 31, 127 ) ;
	size ( 32, 0 ) ;

	/* open the file in appropriate mode */
	in = open ( fname, O_BINARY | O_RDONLY ) ;

	/* if unable to open file */
	if ( in == -1 )
	{
		writestring ( "Error in opening file!", 12, 5, 127 ) ;
		writestring ( "Press any key...", 14, 5, 127 ) ;
		fflush ( stdin ) ;
		getch() ;

		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;
		return ;
	}

	/* calculate area required to store the graph in memory */
	area = 4 + ( ( 640 + 7 ) / 8 ) * 200 ;

	/* read the contents from the file into allocated memory */
	read ( in, graph_ptr, area ) ;

	/* close the file */
	close ( in ) ;

	/* switch to graphics mode */
	graphicsmode() ;
	clearviewport() ;

	/* put the contents of the allocated memory on the screen */
	putimage ( 0, 0, graph_ptr, OR_PUT ) ;

	fflush ( stdin ) ;
	getch() ;

	/* switch back to text mode */
	closegraph() ;
	restorecrtmode() ;

	size ( 32, 0 ) ;
	restorevideo ( 0, 0, 24, 79, p ) ;
	free ( p ) ;
}

/* prints the graph on printer */
print()
{
	char ch, *p ;
	union REGS i, o ;
	int areareqd ;

	/* calculate area required to save entire screen contents */
	areareqd = ( 24 - 0 + 1 ) * ( 79 - 0 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	savevideo ( 0, 0, 24, 79, p ) ;
	menubox ( 9, 3, 16, 75, 127, 07 ) ;

	/* if data doesn't exist */
	if ( max_x_entries < 1 )
	{
		writestring ( "Oops! No data. Go back to Data menu.", 10, 5, 127 ) ;
		writestring ( "Press any key...", 12, 5, 127 ) ;
		fflush ( stdin ) ;
		getch() ;

		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;
		return ;
	}

	writestring ( "Set up the printer and press any key.", 10, 5, 127 ) ;
	writestring ( "Press Esc to return", 12, 5, 127 ) ;

	fflush ( stdin ) ;
	ch = getch() ;

	if ( ch == 27 )  /* Escape key */
	{
		restorevideo ( 0, 0, 24, 79, p ) ;
		free ( p ) ;
		return ;
	}

	menubox ( 9, 3, 16, 75, 127, 07 ) ;
	writestring ( "Please wait...", 10, 12, 127 ) ;
	delay ( 20 ) ;

	/* switch to graphics mode */
	graphicsmode() ;

	/* put the contents of the allocated memory on the screen */
	putimage ( 0, 0, graph_ptr, OR_PUT ) ;

	/* issue interrupt to print the screen contents on the printer */
	int86 ( 5, &i, &o ) ;

	closegraph() ;
	restorecrtmode() ;
	size ( 32, 0 ) ;
	restorevideo ( 0, 0, 24, 79, p ) ;
	free ( p ) ;
}

/* takes control to DOS temporarily */
shell()
{
	int areareqd, status ;
	char *p ;

	/* calculate area required to save entire screen contents */
	areareqd = ( 24 - 0 + 1 ) * ( 79 - 0 + 1 ) * 2 ;

	/* allocate memory, if unsuccessful display error and terminate execution */
	p = malloc ( areareqd ) ;
	if ( p == NULL )
		alloc_error_exit() ;

	savevideo ( 0, 0, 24, 79, p ) ;
	menubox ( 0, 0, 24, 79, 7, 0 ) ;

	menubox ( 8, 21, 16, 60, 127, 47 ) ;
	drawbox ( 9, 23, 14, 56, 127 ) ;
	writestring ( "Quitting temporarily to DOS    ", 11, 25, 127 ) ;
	writestring ( "Type EXIT to return...", 13, 25, 127 ) ;

	gotoxy ( 7, 1 ) ;
	size ( 5, 7 ) ;

	status = system ( "C:\\COMMAND.COM" ) ;

	if ( status == -1 )
	{
		writestring ( "Oops! Cannot load COMMAND.COM!", 11, 25, 127 ) ;
		writestring ( "Press any key to return...", 13, 25, 127 ) ;
		fflush ( stdin ) ;
		getch() ;
	}

	size ( 32, 0 ) ;
	restorevideo ( 0, 0, 24, 79, p ) ;
}

/* calculates time for which Chart Master was active */
activetime()
{
	int hr = 0, min = 0, sec = 0 ;
	long duration ;

	/* find time at end of execution */
	time ( &endtime ) ;

	/* calculate time for which Chart Master was active */
	duration = ( long ) difftime ( endtime, starttime ) ;
	hr = ( int ) duration / 3600 ;
	duration = duration % 3600 ;
	min = ( int ) duration / 60 ;
	sec = ( int ) duration % 60 ;

	/* display the calculated time */
	clrscr() ;
	size ( 5, 7 ) ;
	gotoxy ( 1, 1 ) ;
	printf ( "Thanks for using Chart Master. You were with us for " ) ;
	printf ( "%d hr(s) %d min(s) %d sec(s).\n", hr, min, sec ) ;
}

/* displays memory allocation error and terminates execution */
alloc_error_exit()
{
	writestring ( messages[31], 22, 14, 127 ) ;
	writestring ( messages[32], 23, 14, 127 ) ;
	fflush ( stdin ) ;
	getch() ;

	activetime() ;
	exit ( 3 ) ;
}
