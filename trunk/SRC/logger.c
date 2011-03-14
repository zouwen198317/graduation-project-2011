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
/* log_init: log initialization function. */
int log_init( char *fileName )
{
	/* Make sure that logger wasn't already initiated. Otherwise, return
	 * EMFILE (Error, too many opened files.).
	 */
	if( log_fd )
		return EMFILE;

	/* If a NULL pointer is sent in filename, the default file is used. */
	if( fileName == NULL )
	{
		fileName = (char *) alloca( strlen(DEFAULT_LOG_FILE) );
		fileName = DEFAULT_LOG_FILE;
	}

	/* Opening the log file for writing. Append if existant and create if
	 * non-existant. File is created with permissions 644.
	 */
	if( ( log_fd = open( fileName, O_WRONLY | O_CREAT | O_APPEND, FMODE ) ) < 0 )
	{
		/* In case of error, return errno. */
		return errno;
	}

	/* Initialization succeeded. */
	return 0;
}

/* log_term: terminates the logging. */
int log_term()
{
	if( log_fd < 1 )
		return ENOTSUP;

	/* Closing log file. On error, return errno. */
	if( close(log_fd) < 0 )
	{
		log_write( "Error closing log", strerror( errno ) );
		return errno;
	}

	/* Operation succeeded. */
	return log_fd = 0;
}

/* log_write: log data. */
int log_write( char *title, char *msg )
{
	/* Get current time. */
	time_t timeNow = time( NULL );
	char *timeText = ctime( &timeNow );
	timeText[ strlen( timeText ) -1 ] = '\0';
	/* Calculate message size. */
	int msgSize = strlen( msg ) + strlen( title ) + strlen( timeText ) + 6;
	/* Allocate memory for the message to be printed. */
	char *buff = (char *) malloc( msgSize );
	/* Creating message. */
	snprintf( buff, msgSize, "%s: %s: %s\n", timeText, title, msg );
	/* Writing data to log file. */
	write( ( log_fd > 0 ) ? log_fd : STDERR_FILENO, buff, msgSize );
	/* Freeing allocated memory. */
	free( buff );
	return 0;
}
