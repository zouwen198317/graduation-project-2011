/*
 * The GPS data parser header file.
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

#ifndef	GPS_PARSER_H
#define GPS_PARSER_H

/* Header files. */
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>


/* The GPSData data type is a structure to store GPS data.
 * Where lat and lon represent the latitude and longitude respectively, with
 * North and East taking positive values, and South and West taking negative
 * values.
 */
typedef struct
{
	time_t	timeAndDate;
	bool	valid;
	double	lat;
	double	lon;
	double	speed;
	float	speedDirection;
} GPSData;

/* Functions' Prototypes. */

/* NMEARead function is data parser. It takes two parameters; The first is a
 * pointer to character containing the data line to be parsed. The second
 * parameter is a pointer to a GPSData data type. In addition, NMEARead converts
 * the data to more suitable formats; It converts the longitude and latitude
 * from degree to fractions, the time and date to time_t, and the speed from
 * knots to km/h.
 * Return values:
 * 	+ If the string to parse does not start with "$GPRMC", or contains a
 * 	  incorrect number of fields ( != 13 ), or even if the GPRMC line is
 * 	  correct but data sent from GPS are invalid and the date and time
 * 	  fields are of incorrect length, NMEARead returns a NULL pointer with
 * 	  all the members of GPSDataPointer set to zero/false.
 *	+ If the GPS data are invalid but time and date are of correct length,
 *	  NMEARead returns a NULL pointer with the timeAndDate member of GPSData
 *	  set to the time and date sent, and the valid member is set to false.
 *	+ In case of data valid, NMEARead returns the GPSData pointer passed to
 *	  it in the second parameter with all the members of GPSDataPointer set.
 */
GPSData * NMEARead( char * NMEAstring, GPSData * GPSDataPointer);

#endif
