/*
 * The internet connector code file.
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
#include "internetConnector.h"
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include "logger.h"
#include <wait.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>


/* Definitions. */
#define FORKDELAY 5
#define CHECKDELAY 60
#define CONNECTIONDELAY 10

/* Macros. */
#define LOG( ... ) log_write( "Internet Connector", __VA_ARGS__, NULL )

/* Functions' prototypes. */
bool isconnected();

/* Functions. */
void inetConnect()
{
	pid_t cpid;
	char *argv[] = { "/usr/bin/wvdial", NULL };

	while( 1 )
	{
		LOG( "Forking a new process to execute wvdial." );
		cpid = fork();
		if( cpid < 0 )
		{
			LOG( strerror( errno ) );
			LOG( "Sleeping ", FORKDELAY, " seconds and retrying." );
			sleep( FORKDELAY );
		}
		else if( !cpid )
		{
			int nullfd;
			LOG( "Child: Redirecting STDOUT and STDERR to /dev/null." );
			if( ( nullfd = open( "/dev/null", O_WRONLY ) ) < 0 )
			{
				LOG( strerror( errno ) );
				LOG( "Child: Could not open /dev/null for writing. Skipping redirction.");
			}
			else
			{
				LOG( "Child: Redirecting STDERR." );
				if( dup2( nullfd, STDERR_FILENO ) < 0 )
				{
					LOG( strerror( errno ) );
				}
				LOG( "Child: Redirecting STDOUT." );
				if( dup2( nullfd, STDOUT_FILENO ) < 0 )
				{
					LOG( strerror( errno ) );
				}
			}	
			LOG( "Child: Executing wvdial." );
			execv( argv[ 0 ], argv );
			LOG( strerror( errno ) );
			LOG( "Child: Looping" );
		}
		else
		{
			LOG( "Parent: Monitoring internet connection." );
			sleep( CHECKDELAY );
			while( isconnected() )
				sleep( CHECKDELAY );
			LOG( "Parent: No internet connection is found. Sending wvdial a SIGTERM." );
			if( kill( cpid, SIGTERM ) < 0 )
				LOG( strerror( errno ) );
			waitpid( cpid, NULL, 0 );
			LOG( "Parent: wvdial process died, looping." );
		}
		sleep( CONNECTIONDELAY );
	}

	LOG( "Program flow is not support to reach here. Exiting." );
	return;
}

bool isconnected()
{
	struct addrinfo *res, *p;
	int ret, sockfd;
	char ipstr[INET6_ADDRSTRLEN];

	if( ( ret = getaddrinfo("www.google.com", "80", NULL, &res) ) < 0)
	{
		LOG( gai_strerror( ret ) );
		return false;
	}

	for( p = res; p != NULL; p = p -> ai_next )
	{
		void *addr;

		if( p -> ai_family == AF_INET )
		{
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)( p -> ai_addr );
			addr = &(ipv4->sin_addr);
		}
		else
		{
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)( p -> ai_addr );
			addr = &( ipv6 -> sin6_addr );
		}

		inet_ntop( p -> ai_family, addr, ipstr, sizeof ipstr);
	}

	if( ( sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol) ) < 0 )
	{
		LOG( strerror( errno ) );
		return false;
	}

	if( connect( sockfd, res -> ai_addr, res -> ai_addrlen ) < -1 )
        {
		LOG( strerror( errno ) );
		freeaddrinfo(res);
		close( sockfd );
		return false;
	}
	close(sockfd);
	freeaddrinfo(res);
	return true;
}
