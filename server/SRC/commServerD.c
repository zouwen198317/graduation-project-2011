/*
 * The car-to-server communication code file.
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
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "logger.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include "networkConstants.h"
#include <stdint.h>
#include "gpsXml.h"
#include <limits.h>
#include <sys/stat.h>
#include <signal.h>

/* Definitions. */
#define BUFFSIZ 1024
#define CONNECTION_DELAY 5
#define CONNECTION_ATTEMPTS 15
#define PORT_TRANSMISSION_ATTEMPTS 5
#define PID_FILE "/var/www/pid.text"
#define FIFOLOCK "/var/www/lock"
#define FIFOUNLOCK "/var/www/unlock"

/* Typedefs. */
typedef struct
{
	pid_t pid;
	char car_id[20];
	void *previous;
	void *next;
} id_element;

/* Global variables. */
int STREAM_socket = 0;
id_element main_element;
id_element *last_element = &main_element;

/* Marcos. */
#define LOG( ... ) log_write( "Communication process", __VA_ARGS__, NULL )
#define LOGd( PID, ... ) { char buff[128]; strncpy( buff, "Communication daemon ", 128 ); strncat( buff, itoa( PID ), 128 - strlen( buff ) ); log_write( buff, __VA_ARGS__, NULL );}

/* Function prototypes. */
void init_connection_daemon( char * buffer, struct sockaddr_in saddr, socklen_t );
void _string_analyze( char * buffer, int len );
static void readFifos( int signum, siginfo_t *siginfo, void *context );
static void lockCar( int signum, siginfo_t *siginfo, void *context );
static void unlockCar( int signum, siginfo_t *siginfo, void *context );
int transmit( unsigned char ident, char * msg );
pid_t pidof( char * );

/* Functions. */
int main( void )
{
	int my_socket;
	int ret;
	struct sockaddr_in my_saddr, ret_saddr;
	char * buffer = malloc( BUFFSIZ );
	socklen_t socket_len;
	int optval = 1;
	struct sigaction sigHandle;

	/* Initializing the logger. */
	LOG( "Initializing the log." );
	/* If log wasn't initialized successfully. */
	if( ret = log_init( LOGFILE ) )
		LOG( strerror( ret ) );


	main_element.pid = getpid();
	strcpy( main_element.car_id, "NONE" );
	main_element.previous = main_element.next = NULL;

	LOG( "Writing the process ID in '", PID_FILE, "'." );
	unlink( PID_FILE );
	if( ( ret = open( PID_FILE, O_WRONLY | O_CREAT) ) < 0 )
		LOG( strerror( errno ) );
	else
	{
		if( write( ret, itoa( getpid() ), strlen( itoa( getpid() ) ) ) < 0 )
			LOG( strerror( errno ) );
		close( ret );
	}

	LOG( "Creating lock FIFO file: '", FIFOLOCK, "'." );
	if( mkfifo( FIFOLOCK, 0644) < 0 )
		LOG( strerror( errno ) );

	LOG( "Creating unlock FIFO file: '", FIFOLOCK, "'." );
	if( mkfifo( FIFOUNLOCK, 0644) < 0 )
		LOG( strerror( errno ) );

	LOG( "Setting signal mask for signal 23." );
	sigHandle.sa_sigaction = &readFifos;
	sigemptyset( &sigHandle.sa_mask );
	sigHandle.sa_flags = SA_SIGINFO;
	if( ( sigaction( 23, &sigHandle, NULL ) ) < 0 )
		LOG( "Failed to mask signal 23." );
	else
		LOG( "Signal 23 is masked successfully." );

	LOG( "Setting signal mask for signal 21." );
	sigHandle.sa_sigaction = &lockCar;
	sigemptyset( &sigHandle.sa_mask );
	sigHandle.sa_flags = SA_SIGINFO;
	if( ( sigaction( 21, &sigHandle, NULL ) ) < 0 )
		LOG( "Failed to mask signal 21." );
	else
		LOG( "Signal 21 is masked successfully." );

	LOG( "Setting signal mask for signal 16." );
	sigHandle.sa_sigaction = &unlockCar;
	sigemptyset( &sigHandle.sa_mask );
	sigHandle.sa_flags = SA_SIGINFO;
	if( ( sigaction( 16, &sigHandle, NULL ) ) < 0 )
		LOG( "Failed to mask signal 16." );
	else
		LOG( "Signal 16 is masked successfully." );

	while( ( my_socket = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
		LOG( strerror( errno ) );
	LOG( "DGRAM socket created successfully." );

	if( setsockopt( my_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval ) < 0 )
		LOG( "Couldn't set socket to re-use address." );
	else
		LOG( "Socket set to re-use address successfully." );

	my_saddr.sin_family = AF_INET;
	my_saddr.sin_port = htons( INITPORT );
	my_saddr.sin_addr.s_addr = htonl( INADDR_ANY );

	if( bind( my_socket, (struct sockaddr *) &my_saddr, sizeof( struct sockaddr ) ) < 0 )
	{
		LOG( strerror( errno ) );
		exit( EXIT_FAILURE );
	}
	LOG( "Socket bound." );

	socket_len = sizeof( ret_saddr );
	while( 1 )
	{
		LOG( "Waiting for new data to be received." );
		while( ( ret = recvfrom( my_socket, buffer, BUFFSIZ, 0, (struct sockaddr *) &ret_saddr, &socket_len ) ) < 0 )
			LOG( strerror( errno ) );
		buffer[ ret ] = '\0';
		LOG( "Received \"", buffer, "\" on DGRAM socket." );

		pid_t cpid = fork();
		if( cpid < 0)
		{
			LOG( strerror( errno ) );
		}
		else if( !cpid )
		{
			close( my_socket );
			init_connection_daemon( buffer, ret_saddr, socket_len );
			LOG( "Self-terminating daemon with PID=", itoa( getpid() ), "." );
			free( buffer );
			pid_t pid = getpid();
			for( id_element *temp = &main_element; temp != NULL; temp = temp -> next )
				if( temp -> pid == pid )
				{
					( (id_element *)( temp -> previous ) ) -> next = temp -> next;
					( (id_element *)( temp -> next ) ) -> previous = temp -> previous;
					free( temp );
					break;
				}
			/*TODO: updating database */
			return EXIT_SUCCESS;
		}
		last_element -> next = malloc( sizeof( id_element ) );
		( ( id_element *)( last_element -> next ) ) -> previous = last_element;
		last_element = last_element -> next;
		last_element -> next = NULL;
	}

	LOG( "Outside the main loop. Program flow is not supposed to reach here." );
	close( my_socket );
	free( buffer );
	return EXIT_SUCCESS;
}


void init_connection_daemon( char * buffer, struct sockaddr_in saddr, socklen_t socket_len )
{
	int DGRAM_socket;
	int attempts = 0, attempts2 = 0;
	int ret;
	pid_t my_pid = getpid();
	LOG( "Daemon created with pid=", itoa( my_pid ), "." );
	int optval = 1;
	char * read_buff = malloc( NETWORK_BUFFER_SIZE );

	if( strncmp( buffer, IDENTIFICATION_PATTERN, strlen( IDENTIFICATION_PATTERN ) ) != 0 )
	{
		LOGd( my_pid, "Pattern doesn't match." );
		return;
	}
	LOGd( my_pid, "Pattern matched successfully. Checking car state." );
	/* TODO: Check user existance and state. */
	/* TODO: Check port lists and pick a free one. */

LOG( inet_ntoa( saddr.sin_addr ) ); /* TODO: Remove and store IP. */
	if( ( DGRAM_socket = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
	{
		LOGd( my_pid, strerror( errno ) );
		return;
	}
	LOGd( my_pid, "DGRAM socket created successfully." );

	if( setsockopt( DGRAM_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval ) < 0 )
		LOG( "Couldn't set socket to re-use address." );
	else
		LOG( "Socket set to re-use address successfully." );

	while( 1 )
	{
		saddr.sin_port = htons( INITPORT );
		while( ( sendto( DGRAM_socket, "7654" /* TODO: variable port.*/, 4/* TODO: strlen( buffer )*/, 0, (struct sockaddr *) &saddr, socket_len ) < 0 ) && attempts < PORT_TRANSMISSION_ATTEMPTS )
		{
			LOGd( my_pid, strerror( errno ) );
			attempts++;
		}
		if( attempts == PORT_TRANSMISSION_ATTEMPTS )
		{
			LOGd( my_pid, "Cannot reply to car." );
			return;
		}
		LOGd( my_pid, "Port sent to car." );
		
		LOGd( my_pid, "Sleeping for ", itoa( CONNECTION_DELAY ), " seconds to allow time for the car to listen to the port." );
		sleep( CONNECTION_DELAY );
	
		while( ( STREAM_socket = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
		{
			LOGd( my_pid, strerror( errno ) );
			sleep( 1 );
		}
		LOGd( my_pid, "STREAM socket created successfully." );

		if( setsockopt( STREAM_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval ) < 0 )
			LOG( "Couldn't set socket to re-use address." );
		else
			LOG( "Socket set to re-use address successfully." );
	
		saddr.sin_port = htons( atoi( "7654" /* TODO:Change with variable port. */ ) );
		LOGd( my_pid, "Connecting to car." );
		if( ( connect( STREAM_socket, (struct sockaddr *) &saddr, sizeof( struct sockaddr ) ) < 0 ) && ( attempts2 <  CONNECTION_ATTEMPTS ) )
		{
			LOGd( my_pid, strerror( errno ) );
			attempts2++;
			LOGd( my_pid, "Could not connect to car, resending the port on the DGRAM socket." );
		}
		else if( attempts2 == CONNECTION_ATTEMPTS )
		{
			LOGd( my_pid, "Connection failed ", itoa( CONNECTION_ATTEMPTS ), " times, exiting..." )
			return;
		}
		else
			break;
	}

	LOG( "Connected successfully. Ditching the DGRAM socket." );
	close( DGRAM_socket );

	LOG( "Starting reading loop." );
	while( 1 )
	{
		if( ( ret = read( STREAM_socket, read_buff, NETWORK_BUFFER_SIZE ) ) < 0 )
		{
			if( errno != EINTR )
			{
				LOG( strerror( errno ) );
				close( STREAM_socket );
				STREAM_socket = 0;
				free( read_buff );
				return;
			}
			else
				continue;
		}
		read_buff[ ret ] = '\0';
		LOG( "Received data: '", read_buff, "'." );
		_string_analyze( read_buff, ret );
	}

	LOG( "Program flow got out of the reading loop." );
	close( STREAM_socket );
	STREAM_socket = 0;
	free( read_buff );
	return;
}


void _string_analyze( char * buffer, int len )
{
	unsigned char ident = buffer[0];
	buffer++;
	uint16_t size = _getsize( buffer );

	buffer += 2;

	if( strlen( buffer ) > size )
		LOG( "Extra data transmitted. Continuing..." );
	else if( strlen( buffer ) < size )
	{
		LOG( "Insuffucient data transmitted. Dropping data..." );
		return;
	}

	switch( ident )
	{
		case GPS_IDENT:
		{
			char * temp;
			double lng, lat, spd;
			GPoint newPoint;

			LOG( "GPS data detected. Parsing..." );
			lng = strtol( buffer, &temp, 10 );
			if( lng == LONG_MAX || lng == LONG_MIN )
			{
				LOG( strerror( errno ) );
				LOG( "Dropping data." );
				return;
			}
			LOG( "Longitude fetched." );
			lat = strtol( ++temp, &buffer, 10 );
			if( lat == LONG_MAX || lat == LONG_MIN )
			{
				LOG( strerror( errno ) );
				LOG( "Dropping data." );
				return;
			}
			LOG( "Latitude fetched." );
			spd = strtol( ++buffer, &temp, 10 );
			if( spd == LONG_MAX || spd == LONG_MIN )
			{
				LOG( strerror( errno ) );
				LOG( "Dropping data." );
				return;
			}
			LOG( "Speed fetched." );
			LOG( "Updating the XML file." );
			newPoint.lng = lng;
			newPoint.lat = lat;
			newPoint.spd = spd;
			if( addGpsPoint( newPoint ) == EXIT_FAILURE )
				LOG( "Failed to add point. Dropping..." );
			break;
		}
		default:
			LOG( "Failed to identify data. Dropping." );
	}
}

static void lockCar( int signum, siginfo_t *siginfo, void *context )
{
	if( !STREAM_socket )
		return;
	if( transmit( LOCK_IDENT, "0" ) == EXIT_FAILURE )
	{
		LOGd( getpid(), "Failed to transmit unlock signal." );
	}
	else
	{
		LOGd( getpid(), "Unlock signal transmitted successfully." );
	}
}

static void unlockCar( int signum, siginfo_t *siginfo, void *context )
{
	if( !STREAM_socket )
		return;
	if( transmit( LOCK_IDENT, "1" ) == EXIT_FAILURE )
	{
		LOGd( getpid(), "Failed to transmit lock signal." );
	}
	else
	{
		LOGd( getpid(), "Lock signal transmitted successfully." );
	}
}


static void readFifos( int signum, siginfo_t *siginfo, void *context )
{
	FILE *fd;
	char carid[ 20 ];
	LOG( "Received signal 23. Reading FIFO files." );
	LOG( "Reading lock FIFO file." );
	if( ( fd = fopen( FIFOLOCK, O_RDONLY ) ) < 0 )
	{
		LOG( strerror( errno ) );
	}
	else
	{
		while( fscanf( fd, "%s-", carid ) != EOF )
			kill( pidof( carid ), 21 );
		fclose( fd );
	}

	LOG( "Reading unlock FIFO file." );
	if( ( fd = fopen( FIFOUNLOCK, O_RDONLY ) ) < 0 )
	{
		LOG( strerror( errno ) );
	}
	else
	{
		while( fscanf( fd, "%s-", carid ) != EOF )
			kill( pidof( carid ), 16 );
		fclose( fd );
	}
}

int transmit( unsigned char ident, char * msg )
{
	char * buffer = malloc( 1 + 2 + strlen( msg ) + 1);
	int ret;
	uint16_t size = strlen( msg );

	buffer[ 0 ] = ident;
	buffer[ 2 ] = size / 256;
	buffer[ 1 ] = size % 256;
	strncpy( buffer + 3, msg, strlen( buffer ) - 4 );

	ret = write( STREAM_socket, buffer, strlen( buffer ) ); 
	if( ret < 0 )
	{
		LOG( strerror( errno ) );
		return EXIT_FAILURE;
	}
	else if( ret != strlen( buffer ) )
	{
		LOG( "Warning: not the whole buffer was transmitted." );
		return EXIT_FAILURE;
	}
	LOG( "Buffer transmitted successfully." );
	return EXIT_SUCCESS;
}

pid_t pidof( char * carid )
{
	for( id_element *temp = &main_element; temp != NULL; temp = temp -> next )
		if( strcmp( temp -> car_id, carid ) == 0 )
			return temp -> pid;
	return -1;
}
