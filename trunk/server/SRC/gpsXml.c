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
#include <mcheck.h>

/* Definitions. */
#define MAIN_DIRECTORY "/home/michael/"
#define BUFFSIZ 100

/* Macros. */
#define LOG( ... ) log_write( "GPS XML writer", __VA_ARGS__, NULL );

/* Functions' prototypes. */
int updateGpsFiles();
int init_dir( char * );

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
	sprintf( buffer, "\t<point lat=\"%lf\" lng=\"%lf\" />\n</line>", new_point.lat, new_point.lng );
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

int updateGpsFiles()
{
	char * buffer = malloc( BUFFSIZ );
	char * bufferc;
	char colors[ 43 ];
	char color[ 7 ];
	char * date_buff;
	time_t time_now = time( NULL );
	struct stat st;
	int ret;
	int fd, fdc;
	struct dirent ** namelist;

	strncpy( buffer, MAIN_DIRECTORY, BUFFSIZ );
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
		init_dir( buffer );
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
		init_dir( buffer );
	}
	else
	{
		LOG( "Scanning and sorting files in the directory." );
		if( ( ret = scandir( buffer, &namelist, 0, alphasort ) ) < 0 )
		{
			LOG( strerror( errno ) );
			free( buffer );
			return EXIT_FAILURE;
		}
		if( ret >= 10 )
		{
			char * tempbuff = malloc( strlen( buffer ) + strlen ( namelist[ 2 ] -> d_name ) + 1 );
			strcpy( tempbuff, buffer );
			strcat( tempbuff, namelist[ 2 ] -> d_name );
			LOG( "8 files are present in the directory." );
			LOG( "Deleting: '", tempbuff, "'." );
			if( unlink( tempbuff ) < 0)
			{
				LOG( strerror( errno ) );
				free( tempbuff );
				free( buffer );
				return EXIT_FAILURE;
			}
			free( tempbuff );
		}
	}

	date_buff = malloc( 7 );
	strftime( date_buff, 7, "%y%m%d", localtime( &time_now ) );
	bufferc = malloc( strlen( buffer ) + 7 );
	strncpy( bufferc, buffer, strlen( buffer ) + 1 );
	strncat( bufferc, "colors", 7 );
	strncat( buffer, date_buff, BUFFSIZ - strlen( buffer ) );
	free( date_buff );

	LOG( "Creating a new file: '", buffer, "'." );
	if( ( fd = open( buffer, O_CREAT | O_RDWR, 0644 ) ) < 0 )
	{
		LOG( strerror( errno ) );
		free( bufferc );
		free( buffer );
		return EXIT_FAILURE;
	}
	
	memset( buffer, '\0', BUFFSIZ );
	LOG("Generating line wrappers." );
	if( ( fdc = open( bufferc, O_RDWR ) ) < 0 )
	{
		LOG( strerror( errno ) );
		free( buffer );
		free( bufferc );
		return EXIT_FAILURE;
	}
	read( fdc, colors, 42 );
	colors[ 42 ] = '\0';
	strncpy( color, colors, 6 );
	color[ 6 ] = '\0';
	for( int i = 6; i < 42; i++ )
		colors[ i - 6 ] = colors[ i ];
	for( int i = 36; i < 42; i++ )
		colors[ i ] = color[ i - 36 ];
	if( lseek( fdc, 0, SEEK_SET ) < 0 )
	{
		LOG( strerror( errno ) );
		free( buffer );
		free( bufferc );
		return EXIT_FAILURE;
	}
	if( write( fdc, colors, 42 ) < 0 )
	{
		LOG( strerror( errno ) );
		free( buffer );
		free( bufferc );
		return EXIT_FAILURE;
	}
	close( fdc );
	sprintf( buffer, "<line colour=\"#%s width=\"4\">\n</line>", color );
	if( write( fd, buffer , 40 ) < 0 )
	{
		LOG( strerror( errno ) );
		return EXIT_FAILURE;
	}

	free( buffer );
	close( fd );
	return EXIT_SUCCESS;
}

int init_dir( char * buffer )
{
	int fd;
	char * buff = malloc( BUFFSIZ );
	
	LOG( "Initializing the directory and the files required." );
	if( mkdir( buffer, 0755 ) < 0 )
	{
		LOG( strerror( errno ) );
		return EXIT_FAILURE;
	}
	LOG( "Directory '", buffer, "' created successfully." );

	strcpy( buff, buffer );
	strcat( buff, "colors" );

	if( ( fd = open( buff, O_CREAT | O_RDWR, 0644 ) ) < 0 )
	{
		LOG( strerror( errno ) );
		free( buff );
		return EXIT_FAILURE;
	}
	LOG( "File '", buff, "' created successfully." );
	LOG( "Writing initial data to '", buff, "'." );
	free( buff );
	if( write( fd, "000000FFFFFFFF000000FF000000FFFFFF0000FFFF", 42 ) < 0 )
	{
		LOG( strerror( errno ) );
		return EXIT_FAILURE;
	}
	close( fd );
	return EXIT_SUCCESS;
}
