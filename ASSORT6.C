# include "stdio.h"

FILE *fp ;
main()
{
	char original[9], new[9] ;

	fp = fopen ( "c:\\command.com", "rb+" ) ;
	if ( fp == NULL )
	{
		puts ( "error opening file" ) ;
		exit() ;
	}

	printf ( "\nWhich command do you wish to change ? " ) ;
	scanf  ( "%s", original) ;
	printf ( "\nTo what ? " ) ;
	scanf  ( "%s",new ) ;

	if ( strlen ( original ) != strlen ( new ) )
	{
		printf ( "Enter an alternative command name of the same length" ) ;
		exit() ;
	}

	strupr ( original ) ;
	strupr ( new ) ;
	findcommand ( original, new ) ;
	fclose ( fp ) ;
}

findcommand ( char *s1, char *s2 )
{
	int k = 0, ind = 0, length, ch, flag = 0 ;
	long int i = 0 ;

	length = strlen ( s1 ) ;
	while ( ( ch = getc(fp) ) != EOF )
	{
		if ( ind == k  && ch == s1[k] )
		{
			ind++ ;
			k++ ;
		}
		else
		{
			ind = 0 ;
			k = 0 ;
		}

		i++;
		if ( ind == length )
		{
			flag = 1 ;
			fseek ( fp, ( i - length ), SEEK_SET ) ;
			fputs ( s2, fp ) ;
			ind = 0 ;
			k = 0 ;
		}
	}

	if ( flag != 1 )
	{
		printf ( "No such DOS command" ) ;
		exit ( 1 ) ;
	}
}
