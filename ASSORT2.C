# include "bios.h"

struct partition
{
	unsigned char bootable ;
	unsigned char start_side ;
	unsigned int start_sec_cyl ;
	unsigned char parttype ;
	unsigned char end_side ;
	unsigned int end_sec_cyl ;
	unsigned long part_beg ;
	unsigned long plen ;
} ;

struct part
{
	unsigned char master_boot[446] ;
	struct partition pt[4] ;
	int lasttwo ;
} ;

struct part p ;

main()
{
	unsigned int s_sec, s_trk, e_sec, e_trk, i, t1, t2 ;
	char type[20], boot[5] ;

	biosdisk ( 2, 0x80, 0, 0, 1, 1, &p ) ;
	printf("\nPart.   Boot   Starting location     Ending  Location    Relative  Number of");
	printf("\nType         Side Cylinder Sector  Side Cylinder Sector  Sectors   Sectors\n");

	for ( i = 0 ; i <= 3 ; i++ )
	{
		if ( p.pt[i].bootable == 0x80 )
			strcpy ( boot, "Yes" ) ;
		else
			strcpy ( boot, "No" ) ;

		switch ( p.pt[i].parttype )
		{
			case 0 :
				strcpy ( type, "Unused" ) ;
				break ;

			case 1 :
				strcpy ( type, "12-Bit" ) ;
				break ;

			case 2 :
				strcpy ( type, "Xenix" ) ;
				break ;

			case 4 :
				strcpy ( type, "16-Bit" ) ;
				break ;

			case 5 :
				strcpy ( type, "Extended" ) ;
				break ;

			case 6 :
				strcpy ( type, "Huge" ) ;
				break ;

			default :
				strcpy ( type, "Unknown" ) ;
				break ;
		}

		s_sec = ( p.pt[i].start_sec_cyl & 0x3f )  ;
		t1 = ( p.pt[i].start_sec_cyl & 0xff00 ) >> 8 ;
		t2 = ( p.pt[i].start_sec_cyl & 0x00c0 ) << 2 ;
		s_trk = t1 | t2 ;

		e_sec = ( p.pt[i].end_sec_cyl & 0x3f )  ;
		t1 = ( p.pt[i].end_sec_cyl & 0xff00 ) >> 8 ;
		t2 = ( p.pt[i].end_sec_cyl & 0x00c0 ) << 2 ;
		e_trk = t1 | t2 ;

		printf("\n%6s   %3s", type, boot ) ;
		printf("%4d %6d %8d", p.pt[i].start_side, s_trk,s_sec);
		printf("%7d %6u %8u", p.pt[i].end_side,  e_trk, e_sec);
		printf("%8lu %10lu" , p.pt[i].part_beg,  p.pt[i].plen);
	}
}
