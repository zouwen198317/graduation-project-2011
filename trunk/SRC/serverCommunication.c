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
#include "serverCommunication.h"
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include "logger.h"
#include <arpa/inet.h>
#include <netdb.h>
#include "networkConstants.h"

/* Definitions. */
#define BUFFSIZ 1024
#define CAR_ID "TestID"
#define SERVERNAME "10.0.0.100"//"e-car.dyndns.org"
#define SERVER_TIMEOUT 15

/* Global Variables. */
int STREAM_socket = 0;

/* Functions parameters. */
struct sockaddr *getip( const char * );
void _string_analyze( char * buffer, int len );

/* Macros. */
#define LOG( ... ) log_write( "Communication Process", __VA_ARGS__, NULL )

/* Functions. */
int connectToServer( void )
{
	int STREAM_socket, STREAM_socket2;
	int ret;
	struct sockaddr_in saddr2, saddr3, ret_saddr;
	struct sockaddr saddr;
	char * buffer = malloc( BUFFSIZ );
	socklen_t socket_len;
	fd_set rfds;
	struct timeval time_out; 
	struct sockaddr *serverInfo = getip( SERVERNAME );
	int optval = 1;
	char * read_buff = malloc( NETWORK_BUFFER_SIZE );

	if( serverInfo == NULL )
	{
		LOG( "Could not resolve server's domain name." );
		return EXIT_FAILURE;
	}

	if( ( STREAM_socket = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
	{
		LOG( "Could not resolve server's domain name." );
		return EXIT_FAILURE;
	}

	if( ( STREAM_socket = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
	{
		LOG( strerror( errno ) );
		exit( EXIT_FAILURE );
	}
	LOG( "DGRAM socket created for transmission successfully." );

	if( setsockopt( STREAM_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval ) < 0 )
		LOG( "Couldn't set socket to re-use address." );
	else
		LOG( "Socket set to re-use address successfully." );

	strncpy( buffer, IDENTIFICATION_PATTERN, BUFFSIZ );
	strncat( buffer, CAR_ID, BUFFSIZ - strlen(buffer) );

	while( 1 )
	{
		while( 1 )
		{
//			saddr.sin_family = AF_INET;
//			saddr.sin_addr.s_addr = inet_addr( serverIP );
//			saddr.sin_port = htons( INITPORT );
	
			LOG( "Attempting to send identification pattern to server" );
			if(  ( ret = sendto( STREAM_socket, buffer, strlen( buffer ), 0, serverInfo, sizeof( struct sockaddr ) ) ) < 0 )
				LOG( strerror( errno ) );
			else
				break;
		}
		LOG( "Identification pattern sent to server." );
	

		if( ( STREAM_socket2 = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )
		{
			LOG( strerror( errno ) );
			exit( EXIT_FAILURE );
		}
		LOG( "DGRAM socket created for receiving successfully." );

		if( setsockopt( STREAM_socket2, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval ) < 0 )
			LOG( "Couldn't set socket to re-use address." );
		else
			LOG( "Socket set to re-use address successfully." );

		saddr2.sin_family = AF_INET;
		saddr2.sin_port = htons( INITPORT );
		saddr2.sin_addr.s_addr = htonl( INADDR_ANY );

		if( bind( STREAM_socket2, (struct sockaddr *) &saddr2, sizeof( struct sockaddr ) ) < 0 )
		{
			LOG( strerror( errno ) );
			exit( EXIT_FAILURE );
		}
		LOG( "Socket bound." );

		socket_len = sizeof( ret_saddr );
		FD_ZERO( &rfds );
		FD_SET( STREAM_socket2, &rfds );
		time_out.tv_sec = SERVER_TIMEOUT;
		time_out.tv_usec = 0;
		LOG( "Waiting for server to reply." );
		ret = select( STREAM_socket2 + 1, &rfds, NULL, NULL, &time_out );
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
		if( ( ret = recvfrom( STREAM_socket2, buffer, BUFFSIZ, 0, (struct sockaddr *) &ret_saddr, &socket_len ) ) < 0 )
			LOG( strerror( errno ) );
		else
			break;
	}
	buffer[ ret ] = '\0';
	LOG( "Server replied:\'", buffer, "\'." );

	close( STREAM_socket );
	if( ( STREAM_socket = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		LOG( strerror( errno ) );
		exit( EXIT_FAILURE );
	}
	LOG( "STREAM socket created successfully." );

	if( setsockopt( STREAM_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval ) < 0 )
		LOG( "Couldn't set socket to re-use address." );
	else
		LOG( "Socket set to re-use address successfully." );

	saddr3.sin_family = AF_INET;
	saddr3.sin_addr.s_addr = htonl( INADDR_ANY );
	saddr3.sin_port = htons( atoi( buffer ) );
	while( bind( STREAM_socket, (struct sockaddr *) &saddr3, sizeof( struct sockaddr ) ) < 0 )
	{
		LOG( strerror( errno ) );
		sleep( 1 );
	}
	LOG( "STREAM socket bound successfully." );

	while( listen( STREAM_socket, 1 ) < 0 )
	{
		LOG( strerror( errno ) );
		sleep( 1 );
	}
	LOG( "Listening to the given port and waiting for the incoming connection." );

	socket_len = sizeof( ret_saddr );
	if( ( ret = accept( STREAM_socket, (struct sockaddr *) &ret_saddr, &socket_len ) ) < 0 )
	{
		LOG( strerror( errno ) );
		sleep( 1 );
	}
LOG( strerror( errno ) );

	LOG( "Connection accepted. Starting reading loop." );

//	free( buffer );

	while( 1 )
	{
		if( ( ret = read( STREAM_socket, read_buff, NETWORK_BUFFER_SIZE ) ) < 0 )
		{
			if( errno != EINTR )
			{
				LOG( strerror( errno ) );
				//close( STREAM_socket );
				//STREAM_socket = 0;
				//free( read_buff );
				//return EXIT_FAILURE;
				continue;
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
	return EXIT_SUCCESS;

}


struct sockaddr *getip( const char *domainName )
{
	struct addrinfo *info, *temp;
	int ret, sockfd;
	struct sockaddr *saddr;
	int optval = 1;

	if( ( ret = getaddrinfo( domainName, itoa(INITPORT), NULL, &info ) ) != 0 )
	{
		LOG( gai_strerror( ret ) );
		return NULL;
	}
	LOG( "Domain Name resolved. Testing returned addresses." );

	for( temp = info; temp != NULL;  temp = temp -> ai_next )
	{
		if( ( sockfd = socket( temp -> ai_family, temp -> ai_socktype, temp -> ai_protocol ) ) < 0 )
		{
			LOG( strerror( errno ) );
			continue;
		}

		if( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval ) < 0 )
			LOG( "Couldn't set socket to re-use address." );
		else
			LOG( "Socket set to re-use address successfully." );

		if( connect( sockfd, temp -> ai_addr, temp -> ai_addrlen) < 0 )
		{
			LOG( strerror( errno ) );
			close( sockfd );
			continue;
		}
		close( sockfd );
		break;
	}

	if( temp == NULL )
	{
		LOG( "No valid return addresses were found." );
		return NULL;
	}

	saddr = (struct sockaddr *) malloc( sizeof( struct sockaddr ) );
	memcpy( saddr, temp -> ai_addr, sizeof( struct sockaddr ) );
	freeaddrinfo( info );

	LOG( "A valid address is found: ", inet_ntoa( ( ( struct sockaddr_in *)saddr ) -> sin_addr ), " ." );
	return saddr;
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
		case LOCK_IDENT:
		{
			switch( buffer[ 0 ] )
			{
				case 0:
					/* TODO: lockcar*/
					break;
				case 1:
					/* TODO: unlockcar*/
					break;
			}
			break;
		}
		default:
			LOG( "Failed to identify data. Dropping." );
	}
}

int transmit( unsigned char ident, char * msg )
{
	char * buffer = malloc( 1 + 2 + strlen( msg ) + 1);
	int ret;
	uint16_t size = strlen( msg );

	if( !STREAM_socket )
		return EXIT_FAILURE;
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

