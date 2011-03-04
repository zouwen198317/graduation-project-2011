/*
 * The logger Code file.
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
#include "logger.h"


/* Definitions. */
#define DEFAULT_LOG_FILE	"~/temp_log"
#define FMODE			S_IFMT | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH

/* global variables. */
int	log_fd = 0;

/* Functions. */
int log_init( char *fileName )
{
	if( fileName == NULL )
	{
		fileName = (char *) malloc( strlen(DEFAULT_LOG_FILE) );
		fileName = DEFAULT_LOG_FILE;
	}

	if( ( log_fd = open( fileName, O_WRONLY | O_CREAT | O_APPEND, FMODE ) ) < 0 )
	{
		return errno;
	}

	return 0;
}

int log_term()
{
	int ret;
	if( ( ret = close(log_fd) ) < 0 )
	{
//		log_write( "Error closing log", strerror( errno ) );
		return errno;
	}
	return log_fd=0;
}


int log_write( char *title, char *msg )
{
	time_t timeNow = time( NULL );
	char *timeText = ctime( &timeNow );
	timeText[ strlen( timeText ) -1 ] = '\0';
	int msgSize = strlen( msg ) + strlen( title ) + strlen( timeText ) + 6;
	char *buff = (char *) malloc( msgSize );
	sprintf( buff, "%s: %s: %s\n", timeText, title, msg );
	write( ( log_fd > 0 ) ? log_fd : STDERR_FILENO, buff, msgSize );

}
