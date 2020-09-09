# include "conio.h"
# include "dos.h"

main()
{
	int num, r, c, i = 0 , s = 0 ;
	unsigned char buffer[512] ;

	absread ( 0, 1, 0, buffer ) ;

	for ( num = 1 ; num <= 2 ; num++ )
	{
		clrscr () ;

		gotoxy ( 0, 0 ) ;
		printf ( "\nRelative sector displayed is %06d\n", s ) ;
		gotoxy( 0, 2 ) ;
		printf("\nOffset  ------------------" ) ;
		gotoxy ( 32, 2 ) ;
		printf ( "Hex codes ------------------Ascii values") ;

		for ( r = 6 ; r <= 21 ; r++ )
		{
			gotoxy ( 2, r ) ;
			printf ( "%04d(%04X)", i, i ) ;
			gotoxy ( 14, r ) ;

			/* printing Hex codes */
			for ( c = 0 ; c <= 15  ; c++ )
			{
				printf ( "%02X ", buffer[i] ) ;
				i++ ;
			}

			i = i - 16 ;
			gotoxy ( 64, r ) ;

			/* printing Ascii codes */
			for ( c = 0 ; c <= 15 ; c++ )
			{
				if ( buffer[i] < 32 )
					printf ( " " ) ;
				else
					printf ( "%c", buffer[i] ) ;

				i++ ;
			}
		}

		if ( num == 1 )
		{
			gotoxy ( 1, 24 ) ;
			printf ( "Press any key to continue..." ) ;
			getch () ;
		}
	}

	getch() ;
}
