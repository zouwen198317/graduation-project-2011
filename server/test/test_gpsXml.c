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

/* Header files. */
#include "../SRC/gpsXml.h"
#include <stdlib.h>
#include <string.h>

int main()
{
	car_id = malloc( 10 );
	GPoint new_point;
	strncpy( car_id, "testcarid", 9 );

	new_point.lng = 30.2133;
	new_point.lat = 29.1234;
	new_point.spd = 3.23;
	addGpsPoint( new_point );

	return EXIT_SUCCESS;
}
