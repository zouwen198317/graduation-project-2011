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


/* Header files. */
#include "gpsParser.h"

/* Functions' Prototypes. */
char ** __fieldsSeparator( char * textString, char * fields[13]);

/* Functions' Implementation. */

/* NMEARead (explained in the header file.) */
GPSData * NMEARead( char * NMEAstring, GPSData * GPSDataPointer)
{
	char *		tempChar = NMEAstring;
	struct tm	timestruct;
	char *		fields[13];

	/* Initialization of GPSData struct. */
	*GPSDataPointer = ( GPSData ) { 0, false, 0, 0, 0 };

	/* Returning NULL, if data string is invalid. */
	if( !__fieldsSeparator( NMEAstring, fields ) ||
			strncmp( fields[0], "$GPRMC", 6 ) ||
			( fields[ 2 ][ 0 ] != 'A' &&
			  ( strlen( fields[ 1 ] ) != 10 ||
			    strlen( fields[ 9 ] ) != 6 ) ) )
		return NULL;

	/* Parsing time and date.
	 * The time is stored in field 1 while the date is stored in field9.
	 * Each digit is substracted by 48 to convert from char to int, then the
	 * tens digits are multiplied by ten and then added to the units.
	 */
	timestruct.tm_hour  =	( fields[ 1 ][ 0 ] - 48 ) * 10 +
		fields[ 1 ][ 1 ] - 48;
	timestruct.tm_min   = 	( fields[ 1 ][ 2 ] - 48 ) * 10 +
		fields[ 1 ][ 3 ] - 48;
	timestruct.tm_sec   = 	( fields[ 1 ][ 4 ] - 48 ) * 10 +
		fields[ 1 ][ 5 ] - 48;
	timestruct.tm_mday  =	( fields[ 9 ][ 0 ] - 48 ) * 10 +
		fields[ 9 ][ 1 ] - 48;
	/* tm_mon stores months from 0 to 11, hence an extra 1 is subtracted.*/
	timestruct.tm_mon   = 	( fields[ 9 ][ 2 ] - 48 ) * 10 +
		fields[ 9 ][ 3 ] - 49;
	/* tm_year is the number of years since 1900, hence a 100 is added to
	 * convert to 20XX.
	 */
	timestruct.tm_year  =	( fields[ 9 ][ 4 ] - 48 ) * 10 +
		fields[ 9 ][ 5 ] + 52;
	/* tm_isdst set to '-1' meaning that the usage of DST is unknown. */
	timestruct.tm_isdst =	-1;
	/* Time is converted to time_t and store in GPSDataPointer. */
	GPSDataPointer->timeAndDate = mktime(&timestruct);
	
	/* If the data are invalid, a NULL is returned with only the time and
	 * date set in GPSDataPointer.
	 */
	if( fields[2][0] != 'A' )
		return NULL;

	GPSDataPointer->valid = true;

	/* The rest of the data is parsed and converted. */

	GPSDataPointer->lat = ( ( fields[ 3 ][ 0 ] - 48 ) * 10 +
			fields[	3 ][ 1 ] - 48 + atof( fields[ 3 ] + 2 )
			/ 60 ) * ( ( fields[ 4 ][ 0 ] == 'N' ) ? 1:-1);

	GPSDataPointer->lon = ( ( fields[ 5 ][ 0 ] - 48 ) * 100 +
			( fields[ 5 ][ 1 ] - 48 ) * 10 +
			fields[ 5 ][ 2 ] - 48 +
			atof( fields[ 5 ] + 3 ) / 60 ) *
		( ( fields[ 6 ][ 0 ] == 'E' ) ? 1 : -1 );

	/* Speed is converted from knots to km/h by multiplying by 1.852 */
	GPSDataPointer->speed = atof( fields[ 7 ] ) * 1.852 ;

	return GPSDataPointer;
	
}


/* __fieldsSeparator:
 * It is used to separate the fields in the data string sent. It takes two
 * parameters; The first is a char pointer containing the string to be parsed,
 * the second is an array of char pointers to store the separated fields. Fields
 * are comma separated.
 * Return value:
 * 	+ If the number of fields in the data string is less than 13, it returns
 * 	  a NULL pointer.
 *	+ On success, it returns a pointer to array of fields given in the
 *	  second parameter.
 */
char ** __fieldsSeparator( char * textString, char * fields[13])
{
	/* Main loop; Twelve iterations for twelve commas separating between
	 * thirteen fields.
	 * Note: At the beginning textString points to the beginning of the
	 * 	 first field (the beginning of the whole data string).
	 */
	for( int i = 0; i < 12; i++)
	{
		/* Current field begins with textString. */
		fields[ i ] = textString;

		/* Looking for the next comma and assigning its position to
		 * textString.
		 */
		if( !( textString = strchr( textString, ',') ) )
			return NULL; /* returnin NULL when no comma found. */

		/* Replacing the comma by a '\0', to indicate the end of the
		 * current field, and increasing textString by one.
		 */
		*(textString++) = '\0';
	}

	/* Assigning last field. */
	fields[ 12 ] = textString;

	return fields;
}
