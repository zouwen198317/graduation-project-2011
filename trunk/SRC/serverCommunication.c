/*
 * The server-to-car communication code file.
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
#include <sys/select.h>
#include "logger.h"

/* Definitions. */
#define IDENTIFICATION_PATTERN "#3c4r1d3n74ndr3d1rm3@"
#define BUFFSIZ 1024
#define CAR_ID "TestID"
#define SERVERNAME "e-car.dyndns.org"
#define INITPORT 6543
#define SERVER_TIMEOUT 15

/* TODO: remove when Domain Name resolution is implemented. */
#define temp_server_ip "10.0.0.102"

/* Macros. */
#define LOG( ... ) log_write( "Communication Process", __VA_ARGS__, NULL )

/* Functions. */
int main( void )
{
	int my_socket, my_socket2;
	int ret;
	struct sockaddr_in saddr, saddr2, saddr3, ret_saddr;
	char * buffer = malloc( BUFFSIZ );
	socklen_t socket_len;
	fd_set rfds;
	struct timeval time_out; 

	if( ( my_socket = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
	{
		LOG( strerror( errno ) );
		exit( EXIT_FAILURE );
	}
	LOG( "DGRAM socket created for transmission successfully." );

	strncpy( buffer, IDENTIFICATION_PATTERN, BUFFSIZ );
	strncat( buffer, CAR_ID, BUFFSIZ - strlen(buffer) );

	while( 1 )
	{
		while( 1 )
		{
			saddr.sin_family = AF_INET;
			saddr.sin_addr.s_addr = inet_addr( temp_server_ip );
			saddr.sin_port = htons( INITPORT );
	
			LOG( "Attempting to send identification pattern to server" );
			if( ret = sendto( my_socket, buffer, strlen( buffer ), 0, (struct sockaddr *) &saddr, sizeof( struct sockaddr ) ) < 0 )
				LOG( strerror( errno ) );
			else
				break;
		}
		LOG( "Identification pattern sent to server." );
	

		if( ( my_socket2 = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
		{
			LOG( strerror( errno ) );
			exit( EXIT_FAILURE );
		}
		LOG( "DGRAM socket created for receiving successfully." );

		saddr2.sin_family = AF_INET;
		saddr2.sin_port = htons( INITPORT );
		saddr2.sin_addr.s_addr = htonl( INADDR_ANY );

		if( bind( my_socket2, (struct sockaddr *) &saddr2, sizeof( struct sockaddr ) ) < 0 )
		{
			LOG( strerror( errno ) );
			exit( EXIT_FAILURE );
		}
		LOG( "Socket bound." );

		socket_len = sizeof( ret_saddr );
		FD_ZERO( &rfds );
		FD_SET( my_socket2, &rfds );
		time_out.tv_sec = SERVER_TIMEOUT;
		time_out.tv_usec = 0;
LOG( itoa(ntohs( saddr.sin_port ) ) );
		LOG( "Waiting for server to reply." );
		ret = select( my_socket2 + 1, &rfds, NULL, NULL, &time_out );
		if( ret < 0 )
		{
			LOG( strerror( errno ) );
			continue;
		}
		else if ( !ret )
		{
			LOG( "No answer received from server within ", itoa( SERVER_TIMEOUT ), " seconds." );
			LOG( "Retrying..." );
			continue;
		}
		if( ( ret = recvfrom( my_socket2, buffer, BUFFSIZ, 0, (struct sockaddr *) &ret_saddr, &socket_len ) ) < 0 )
			LOG( strerror( errno ) );
		else
			break;
	}
	buffer[ ret ] = '\0';
	LOG( "Server replied:\'", buffer, "\'." );

	close( my_socket );
	if( ( my_socket = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		LOG( strerror( errno ) );
		exit( EXIT_FAILURE );
	}
	LOG( "STREAM socket created successfully." );

	saddr3.sin_family = AF_INET;
	saddr3.sin_addr.s_addr = inet_addr( temp_server_ip );
	saddr3.sin_port = htons( atoi( buffer ) );

	while( bind( my_socket, (struct sockaddr *) &saddr3, sizeof( struct sockaddr ) ) < 0 )
	{
		LOG( strerror( errno ) );
		sleep( 1 );
	}
	LOG( "STREAM socket bound successfully." );

	while( listen( my_socket, 1 ) < 0 )
	{
		LOG( strerror( errno ) );
		sleep( 1 );
	}
	LOG( "Listening to the given port and waiting for the incoming connection." );

	socket_len = sizeof( ret_saddr );
	if( ( ret = accept( my_socket, (struct sockaddr *) &ret_saddr, &socket_len ) ) < 0 )
	{
		LOG( strerror( errno ) );
		sleep( 1 );
	}
	LOG( "Connection accepted." );



	close( my_socket );

	return 0;
}