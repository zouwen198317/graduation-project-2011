/*
 * The GPS-to-XML code file.
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
#include "gpsXml.h"
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include "logger.h"

/* Definitions. */
#define MAIN_DIRECTORY "/home/michael/"
#define BUFFSIZ 100

/* Macros. */
#define LOG( ... ) log_write( "GPS XML writer", __VA_ARGS__, NULL );

/* Functions' prototypes. */
void updateGpsFiles();

/* Functions. */
int addGpsPoint( GPoint new_point )
{
	if( !car_id )
		return EXIT_FAILURE;

	char * buffer = malloc( BUFFSIZ );
	DIR * cwd;
	struct dirent ** namelist;
	int ret;

	strncat( buffer, MAIN_DIRECTORY, BUFFSIZ );
	strncat( buffer, car_id, BUFFSIZ - strlen( car_id ) );
	strncat( buffer, "/", 1 );

	LOG( "Main directory name = '", buffer, "'." );

	while( !( cwd = opendir( buffer ) ) )
		if( errno == ENOENT )
		{
			LOG( strerror( errno ) );
			if( mkdir( buffer, 0755 ) < 0 )
			{
				LOG( strerror( errno ) );
				return EXIT_FAILURE;
			}
			LOG( "Directory '", buffer, "' created successfully." );
		}
		else
		{
			LOG( strerror( errno ) );
			return EXIT_FAILURE;
		}
	LOG( "Directory '", buffer, "' opened successfully." );

	LOG( "Looking for a file corresponding to the current date." );
	if( ( ret =scandir( buffer, &namelist, 0, alphasort ) ) < 0 )
	{
		LOG( strerror( errno ) );
		return EXIT_FAILURE;
	}


}

void updateGpsFiles()
{


}
