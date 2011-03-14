/*
 * The Main starter (parent) code.
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
#include "gpsProcess.h"
#include "logger.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>


/* Definitions. */
#define LOGFILE		"Logfile"

/* Macros. */
#define LOG( x )	log_write( "Main parent", x )



/* Functions. */
int main()
{
	int ret;
	pid_t pid_gps; /* Children processes PIDs. */


	/* Initializing the logger. */
	ret = log_init( LOGFILE );
	LOG( "Initializing the log." );
	/* If log wasn't initialized successfully. */
	if( ret )
		LOG( strerror( ret ) );
	
	/* Starting the forking phase. */
	pid_gps = fork();
	if( pid_gps < 0 )
	{
		/* Error in forking. */
		LOG( "Process failed to fork, quiting." );
		log_term();
		exit( EXIT_FAILURE );
	}
	if( !pid_gps )
	{
		/* Fork() succeeded, Child process.
		 * GPS Process.
		 */
		LOG( "Process forked successfully, running gpsProcess from child." );
		/* Starting gpsProcess. */
		gpsProcess();
		/* Process exited. */
		/* TODO: not supposed to get back here. */
		LOG( "gpsProcess terminated." );
		exit( EXIT_SUCCESS );
	}
	/* Parent Process. */


	LOG( "Parent process will sleep for 30 seconds." );
	sleep( 30 );
	LOG( "Parent woke up. Will terminate child now." );
	kill( pid_gps, SIGTERM );
	LOG( "Waiting for child to terminate." );
	waitpid( pid_gps, NULL, 0 );
	LOG( "Closing log file." );
	log_term();

	return EXIT_SUCCESS;
}

