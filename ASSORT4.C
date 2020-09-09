# include "conio.h"
# include "dos.h"
# include "bios.h"

# define Esc   0x11B
# define Right 0x4D00
# define F1    0x3B00
# define F2    0x3C00

unsigned char buffer[512], temp[512] ;
int lsect, maxc, i, x, y, indicator ;

main()
{
	int drive ;

	clrscr() ;
	printf ( "\nSelect drive ( 0=A, 1=B, 2=C, etc.) " ) ;
	scanf  ( "%d", &drive) ;
	printf ( "\nEnter logical sector you wish to edit ? " ) ;
	scanf  ( "%d", &lsect ) ;

	absread ( drive, 1, lsect, buffer ) ;
	display() ;
	edit() ;
}

display()
{
	int r, c, i = 0 ;
	unsigned char ch ;

	clrscr() ;
	printf ( "Relative sector displayed is %06d", lsect ) ;
	printf ( "\n --------------------- Hex codes " ) ;
	printf ( "---------------------   Ascii values" ) ;
	for( r = 3 ; r <= 24 ; r++ )
	{
		for( c = 2 ; c <= 55  ; c += 2 )
		{
			ch = buffer[i] ;
			gotoxy ( c, r ) ;
			printf ( "%02X", ch ) ;
			gotoxy ( 57 + ( i % 24 ), r ) ;

			if ( ch == 7 || ch == 8 || ch == 13 || ch == 10 )
				printf (" " ) ;
			else
				printf( "%c", ch ) ;

			i++ ;

			if ( i % 4 == 0 )
				c++ ;

			if ( i == 512 )
			{
				maxc = c ;
				break ;
			}
		}
	}

	printf ( "\nF1-Restore ; F2-Save Changes ; Esc - Exit" ) ;
}

edit()
{
	int ch, ch1, j ;

	x = 2 ;
	y = 3 ;
	indicator = 1 ;
	i = 0 ;

	for ( j = 0 ; j <= 511 ; j++ )
		temp[j] = buffer[j] ;

	while ( 1 )
	{
		gotoxy ( x, y ) ;
		ch = bioskey ( 0 ) ;
		ch1 = toupper ( ch ) ;

		if ( ch1 >= '0' && ch1 <= '9' || ch1 >= 'A' && ch1 <= 'F' )
		{
			ch1 = ch1 - 48 ;

			if ( ch1 > 9 )
				ch1 = ch1 - 7 ;

			if ( indicator == 1 )
			{
				ch1 = ch1 << 4 ;
				temp[i] = temp[i] & 0x0f ;
			}
			else
				temp[i] = temp[i] &  0xf0 ;

			temp[i] = temp[i] | ch1 ;
			putch ( toupper ( ch )) ;
			gotoxy ( (57 + (i % 24)), y ) ;
			printf ( "%c", temp[i] ) ;
			update() ;
		}
		else
		{
			switch ( ch )
			{
				case Right :
					update() ;
					break ;

				case F1  :
					display() ;
					return ;

				case F2 :
					for ( j = 0 ; j <= 511 ; j++ )
					buffer[j] = temp[j] ;
					abswrite ( 0, 1, lsect, temp ) ;
					display() ;
					return ;

				case Esc :
					return ;
			}
		}
	}
}

update()
{
	x++ ;
	indicator = !( indicator ) ;

	if( x >= 55 )
	{
		x = 2 ;
		y++ ;
	}

	if ( y == 24 && x == maxc )
	{
		y = 3 ;
		i = 0 ;
		x = 2 ;
	}

	if ( indicator == 1 )
	{
		i++ ;
		if ( ( x != 2 ) && ( i % 4 == 0 ) )
			x++ ;
	}
}
