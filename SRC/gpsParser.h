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

#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct
{
	time_t	timeAndDate;
	bool	valid;
	double	lat;
	double	lon;
	double	speed;
} GPSData;

GPSData * NMEARead( char * NMEAstring, GPSData * GPSDataPointer);
char ** fieldsSperator( char * textString, char * fields[13]);


#endif
