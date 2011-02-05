/*
 * The GPS data parser Code file.
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



#include "gpsParser.h"


GPSData * NMEARead( char * NMEAstring, GPSData * GPSDataPointer)
{
	char *		tempChar = NMEAstring;
	struct tm	timestruct;
	char *		fields[13];

	if( !fieldsSperator( NMEAstring, fields ) || strncmp( fields[0], "$GPRMC", 6 ) )
	{
		return NULL;
	}
	else if ( fields[ 2 ][ 0 ] != 'A' && ( strlen( fields[ 1 ] ) != 10 || strlen( fields[ 9 ] ) != 6 ) )
	{
		GPSDataPointer->valid = false;
		return NULL;
	}
	else
	{
		//Parsing time and date.
		timestruct.tm_hour  =	( fields[ 1 ][ 0 ] - 48 ) * 10 + fields[ 1 ][ 1 ] - 48;
		timestruct.tm_min   = 	( fields[ 1 ][ 2 ] - 48 ) * 10 + fields[ 1 ][ 3 ] - 48;
		timestruct.tm_sec   = 	( fields[ 1 ][ 4 ] - 48 ) * 10 + fields[ 1 ][ 5 ] - 48;
		timestruct.tm_mday  =	( fields[ 9 ][ 0 ] - 48 ) * 10 + fields[ 9 ][ 1 ] - 48;
		timestruct.tm_mon   = 	( fields[ 9 ][ 2 ] - 48 ) * 10 + fields[ 9 ][ 3 ] - 49;
		timestruct.tm_year  =	( fields[ 9 ][ 4 ] - 48 ) * 10 + fields[ 9 ][ 5 ] + 52;
		timestruct.tm_isdst =	-1;
		GPSDataPointer->timeAndDate = mktime(&timestruct);
		
		if( fields[2][0] != 'A' )
		{
			GPSDataPointer->valid = false;
			return NULL;
		}
		else
		{
			GPSDataPointer->valid = true;
			GPSDataPointer->lat = ( ( fields[ 3 ][ 0 ] - 48 ) * 10 + fields[ 3 ][ 1 ] - 48 + atof( fields[ 3 ] + 2 ) / 60 ) * ( ( fields[ 4 ][ 0 ] == 'N' ) ? 1 : -1 );
			GPSDataPointer->lon = ( ( fields[ 5 ][ 0 ] - 48 ) * 100 + ( fields[ 5 ][ 1 ] - 48 ) * 10 + fields[ 5 ][ 2 ] - 48 + atof( fields[ 5 ] + 3 ) / 60 ) * ( ( fields[ 6 ][ 0 ] == 'E' ) ? 1 : -1 );
			GPSDataPointer->speed = atof( fields[ 7 ] ) * 1.852 ;
			return GPSDataPointer;
		}
	}
}


char ** fieldsSperator( char * textString, char * fields[13])
{
	for( int i = 0; i < 12; i++)
	{
		fields[ i ] = textString;
		if( !( textString = strchr( textString, ',') ) )
			return NULL;
		*(textString++) = '\0';
	}
	fields[ 12 ] = textString;
	return fields;
}
