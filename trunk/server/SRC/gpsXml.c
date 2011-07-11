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
#include <sys/stat.h>
#include <dirent.h>
#include "logger.h"
#include <stdio.h>

/* Definitions. */
#define MAIN_DIRECTORY "/home/michael/"
#define BUFFSIZ 100

/* Macros. */
#define LOG( ... ) log_write( "GPS XML writer", __VA_ARGS__, NULL );

/* Functions' prototypes. */
int updateGpsFiles();

/* Functions. */
int addGpsPoint( GPoint new_point )
{
	if( !car_id )
		return EXIT_FAILURE;

	char * buffer = malloc( BUFFSIZ );
	char * date_buff = malloc( 7 );
	int fd;
	time_t time_now = time( NULL );
	off_t offset;

	strftime( date_buff, 7, "%y%m%d", localtime( &time_now ) );
	strncat( buffer, MAIN_DIRECTORY, BUFFSIZ );
	strncat( buffer, car_id, BUFFSIZ - strlen( buffer ) );
	strncat( buffer, "/", 1 );
	strncat( buffer, date_buff, BUFFSIZ - strlen( buffer ) );
	free( date_buff );

	LOG( "Attempting to open file '", buffer, "'." );
	while( ( fd = open( buffer, O_WRONLY ) ) < 0 )
	{
		if( errno != ENOENT )
		{
			LOG( strerror( errno) );
			free( buffer );
			return EXIT_FAILURE;
		}
		LOG( "File not found. Running directory update." );
		if( updateGpsFiles() < 0 )
		{
			free( buffer );
			return EXIT_FAILURE;
		}
	}
	LOG( "File opened successfully." );
	LOG( "Seeking to the right position." );
	if( (offset = lseek( fd, 0, SEEK_END) ) < 0 )
	{
		LOG( strerror( errno ) );
		close( fd );
		free( buffer );
		return EXIT_FAILURE;
	}
	if( lseek( fd, offset - 7, SEEK_SET ) < 0 )
	{
		LOG( strerror( errno ) );
		close( fd );
		free( buffer );
		return EXIT_FAILURE;
	}

	LOG( "Adding new point." );
	memset( buffer, '\0', BUFFSIZ );
	sprintf( buffer, "\t<point lat=\"%d\" lng=\"%d\" />\n</line>", new_point.lat, new_point.lng );
	if( write( fd, buffer, strlen( buffer ) ) < 0 )
	{
		LOG( strerror( errno ) );
		close( fd );
		free( buffer );
		return EXIT_FAILURE;
	}

	LOG( "Closing file." );
	close( fd );
	free( buffer );
	return EXIT_SUCCESS;
}

int temp_func()
{
	if( !car_id )
		return EXIT_FAILURE;

	char * buffer = malloc( BUFFSIZ );
	char * date_buff = malloc( 7 );
	char * filename;
	DIR * cwd;
	struct dirent ** namelist;
	int ret;
	time_t time_now = time( NULL );

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

	strftime( date_buff, 7, "%y%m%d", localtime( &time_now ) );
	LOG( "Attempting to open the file ", date_buff, "." );


}

int updateGpsFiles()
{
	char * buffer = malloc( BUFFSIZ );
	char * date_buff;
	time_t time_now = time( NULL );
	struct stat st;

	strncat( buffer, MAIN_DIRECTORY, BUFFSIZ );
	strncat( buffer, car_id, BUFFSIZ - strlen( buffer ) );
	strncat( buffer, "/", 1 );

	LOG( "Checking directory '", buffer, "'." );
	if( stat( buffer, &st ) < 0 )
	{
		if( errno != ENOENT )
		{
			LOG( strerror( errno ) );
			free( buffer );
			return EXIT_FAILURE;
		}
		LOG( "Directory does not exist." );
		if( mkdir( buffer, 0755 ) < 0 )
		{
			LOG( strerror( errno ) );
			free( buffer );
			return EXIT_FAILURE;
		}
		LOG( "Directory '", buffer, "' created successfully." );
	}
	else if( !( st.st_mode & S_IFDIR ) )
	{
		LOG( "Another file exist under the name '", buffer, "'." );
		LOG( "Renaming the file to '", buffer, ".file'." );
		char * new_name = malloc( strlen( buffer ) + 6 );
		strcpy( new_name, buffer );
		strcat( new_name, ".file" );
		if( rename( buffer, new_name ) < 0 )
		{
			LOG( strerror( errno ) );
			free( new_name );
			free( buffer );
			return EXIT_FAILURE;
		}
		free( new_name );
		if( mkdir( buffer, 0755 ) < 0 )
		{
			LOG( strerror( errno ) );
			free( buffer );
			return EXIT_FAILURE;
		}
		LOG( "Directory '", buffer, "' created successfully." );
	}

	date_buff = malloc( 7 );
	strftime( date_buff, 7, "%y%m%d", localtime( &time_now ) );
	strncat( buffer, date_buff, BUFFSIZ - strlen( buffer ) );
	free( date_buff );

/*	LOG( "Looking for a file called ", date_buff, "." );

if( ( ret = scandir( buffer, &namelist, 0, alphasort ) ) < 0 )
	{
		LOG( strerror( errno ) );
		return EXIT_FAILURE;
	}

	if( strncmp( namelist[ --ret ] -> d_name, date_buff, 6 ) != 0 )
	{
		LOG( "Not found!" );
	}else
	LOG( "Found!" );
*/


}
