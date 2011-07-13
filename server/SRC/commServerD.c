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

/* Definitions. */
#define IDENTIFICATION_PATTERN "#3c4r1d3n74ndr3d1rm3@"
#define BUFFSIZ 1024
#define INITPORT 6543
#define CONNECTION_DELAY 2
#define CONNECTION_ATTEMPTS 15
#define PORT_TRANSMISSION_ATTEMPTS 5

/* Marcos. */
#define LOG( ... ) log_write( "Communication process", __VA_ARGS__, NULL )
#define LOGd( PID, ... ) { char buff[128]; strncpy( buff, "Communication daemon ", 128 ); strncat( buff, itoa( PID ), 128 - strlen( buff ) ); log_write( buff, __VA_ARGS__, NULL );}

/* Function prototypes. */
void init_connection_daemon( char * buffer, struct sockaddr_in saddr, socklen_t );

/* Functions. */
int main( void )
{
	int my_socket, ret_socket;
	int ret;
	struct sockaddr_in my_saddr, ret_saddr;
	char * buffer = malloc( BUFFSIZ );
	socklen_t socket_len;

	if( ( my_socket = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
	{
		LOG( strerror( errno ) );
		exit( EXIT_FAILURE );
	}
	LOG( "DGRAM socket created successfully." );

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
			return EXIT_SUCCESS;
		}
	}

	close( my_socket );
	free( buffer );
	return EXIT_SUCCESS;
}


void init_connection_daemon( char * buffer, struct sockaddr_in saddr, socklen_t socket_len )
{
	int DGRAM_socket, STREAM_socket;
	int attempts = 0, attempts2 = 0;
	pid_t my_pid = getpid();
	LOG( "Daemon created with pid=", itoa( my_pid ), "." );

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
	while( 1 )
	{
		saddr.sin_port = htons( INITPORT );
LOG( itoa(ntohs( saddr.sin_port ) ));
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
	
		/* TODO: Start TCP connection. */
		saddr.sin_port = htons( atoi( "7654" /* TODO:Change with variable port. */ ) );
		LOGd( my_pid, "Connecting to car." );
LOG( ntohs( saddr.sin_port ) );
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




}