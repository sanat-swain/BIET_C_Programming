# include "stdio.h"
# include "dir.h"
# include "string.h"

int counter = 0 ;
char field[25], path[80], fileinfo[80] ;
struct ffblk file ;

main ( int argc, char *argv[] )
{
	if ( argc != 3 )
	{
		puts ( "Correct usage Mydel <matching field> <path>" );
		exit ( 1 ) ;
	}

	strcpy ( field, argv[1] ) ;
	strcpy ( path,  argv[2] ) ;
	process_files() ;
}

process_files()
{
	int flag ;

	flag = findfirst ( path, &file, 0 ) ;
	if ( flag == -1 )
	{
		printf ( "File not found" ) ;
		exit ( 1 ) ;
	}

	stuff_file_info () ;

	if ( strstr ( fileinfo, field ) != NULL )
		deletefile() ;

	while ( ( flag = findnext ( &file ) ) != -1 )
	{
		stuff_file_info() ;
		if ( strstr ( fileinfo, field ) != NULL )
			deletefile() ;
	}

	printf("\nTotal number of files deleted: %d\n", counter ) ;
}

deletefile()
{
	if ( unlink ( file.ff_name ) ==  -1 )
		printf ( "%s could not be deleted\n", file.ff_name ) ;
	else
		counter++ ;
}

stuff_file_info()
{
	unsigned int yr, mth, day, hr, min ;

	yr =  80 + ( file.ff_fdate >> 9 ) ;
	mth =  ( file.ff_fdate << 7 ) >>  12 ;
	day =  ( file.ff_fdate << 11 ) >> 11 ;
	hr = file.ff_ftime >> 11 ;
	min = ( file.ff_ftime << 5 ) >> 10 ;
	sprintf ( fileinfo, "%-13s\t%10ld\t%02u-%02u-%02u\t%02u:%02u\n", file.ff_name, file.ff_fsize, day, mth, yr, hr, min ) ;
}
