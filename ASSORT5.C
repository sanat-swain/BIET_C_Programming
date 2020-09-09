/* compile this program in large memory model of Turbo C */

#include "bios.h"
#include "dos.h"
#include "alloc.h"
#include "conio.h"

main()
{
	struct boot
	{
		char x[19] ;
		int tot_sec ;
		char y[3] ;
		int sptrk  ;
		int no_of_sides ;
		char rest[484] ;
	} b ;

	int sor, tar, head, trk, sec, i, s, c, flg, col, cyl ;
	void *d[80] ;

	clrscr() ;
	printf ( "Enter source drive ( 0 = A, 1 = B )" ) ;
	scanf ( "%d", &sor ) ;
	printf ( "Enter target drive ( 0 = A, 1 = B )" ) ;
	scanf ( "%d", &tar ) ;
	puts ( "Enter source disk & press any key..." ) ;
	getch() ;

	absread ( sor, 1, 0, &b ) ;
	cyl = b.tot_sec / ( b.sptrk * b.no_of_sides ) ;
	gotoxy ( 20, 10 ) ;
	printf ( "   1    2    3" ) ;
	gotoxy ( 20, 11 ) ;
	printf("Track   0123456789012345678901234567890123456789" );
	gotoxy ( 20, 12 ) ;
	printf ( "side 0" ) ;
	gotoxy ( 20, 13 ) ;
	printf ( "side 1" ) ;

	head = trk = i = 0 ;
	sec = 1 ;
	col = 28 ;

	while ( trk < cyl )
	{
		d[i] = malloc ( b.sptrk * 512 ) ;

		if ( d[i] == NULL )
		{
			printf ( "Memory allocation error" ) ;
			exit ( 1 ) ;
		}

		gotoxy ( col, 12 + head ) ;
		printf ( "R" ) ;
		flg = biosdisk ( 2, sor, head, trk, sec, b.sptrk, d[i] ) ;

		if ( flg != 0 )
		{
			gotoxy ( col, 12 + head ) ;
			printf ( "E" ) ;
		}

		i++ ;
		head = !head ;
		if ( head == 0 )
		{
			trk++ ;
			col++ ;
		}
	}

	puts ( "\nInsert target disk & press any key..." ) ;
	getch() ;
	head = trk = i = 0 ;
	sec = 1 ;
	col = 28 ;

	while ( trk < cyl )
	{
		gotoxy ( col, 12 + head ) ;
		printf ( "W" ) ;
		flg = biosdisk ( 3, tar, head, trk, sec, b.sptrk, d[i] ) ;
		gotoxy ( col, 12 + head ) ;
		flg != 0 ? printf ( "E" ) : printf ( "." ) ;
		i++ ;
		head = !head ;

		if ( head == 0 )
		{
			trk++ ;
			col++ ;
		}
	}
}
