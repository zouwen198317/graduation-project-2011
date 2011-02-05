/*
 * The GPS data parser testing code.
 * Copyright (C) 2011  Michael M. Behman (michael.behman@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include "../SRC/gpsParser.h"


int main( int argc, char * argv[] )
{
	FILE 	*nmeaFile,	*outFile ;
	char 	*textLine = ( char * ) malloc( 1024 );

	if( argc != 2 )
	{
		fprintf( stderr, "%s: missing file operand.\n", argv[0] );
		fprintf( stderr, "Usage:\t'%s file-name'.\n\tWhere file-name is the NMEA formatted file.\n", argv[0] );
		exit( EXIT_FAILURE );
	}

	if( !( nmeaFile = fopen( argv[ 1 ], "r" ) ) )
	{
		perror( NULL );
		exit( EXIT_FAILURE );
	}

	if( !( outFile = fopen( strcat( argv[ 1 ], ".out" ), "w" ) ) )
	{
		perror( NULL );
		exit( EXIT_FAILURE );
	}

	printf( "File opened successfully.\n" );
	printf( "Finding GPRMC lines and parsing data.\n" );

	fprintf( outFile, "\tTime\t\t\tLatitude\tLongitude\tspeed.\n" );
	for( int lineCount = 0; fgets( textLine, 1024, nmeaFile ); lineCount++ )
	{
		char	*timeString;
		GPSData	gps;

		NMEARead( textLine, &gps);
		timeString = ctime(&(gps.timeAndDate));
		timeString[ strlen( timeString ) - 1 ] = '\0';

		if( !gps.valid )
			fprintf( outFile, "%s:\tN/A\t\tN/A\t\tN/A.\n", timeString );
		else
			fprintf( outFile, "%s:\t%lf\t%lf\t%f.\n", timeString, gps.lat, gps.lon, gps.speed );

	}

	fclose( outFile );
	fclose( nmeaFile );
	free( textLine );
	return 0;
}


