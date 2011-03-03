/*
 * The GPS data grabber testing code.
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
#include "../SRC/gpsProcess.h"
#include "../SRC/logger.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>


/* Definitions. */
#define LOGFILE		"extra/gpsProcessTestLog"
/* Macros. */
#define LOG( x )	log_write( "gpsProcess test", x )


/* Functions. */
int main()
{
	int ret;
	pid_t cpid;
	if( ret = log_init( LOGFILE ) )
	{
		printf( strerror( ret ) );
		exit( EXIT_FAILURE );
	}
	
	cpid = fork();
	if( cpid < 0 )
	{
		LOG( "Process failed to fork, quiting." );
		log_term();
		exit( EXIT_FAILURE );
	}
	if( !cpid )
	{
		LOG( "Process forked successfully, running gpsProcess from child." );
		gpsProcess();
		LOG( "gpsProcess terminated." );
		exit( EXIT_SUCCESS );
	}
	else
	{
		LOG( "Parent process will sleep for 30 seconds." );
		sleep( 30 );
		LOG( "Parent woke up. Will terminate child now." );
		kill( cpid, SIGTERM );
	}
	LOG( "Waiting for child to terminate." );
	waitpid( cpid, NULL, 0 );
	LOG( "Closing log file." );
	log_term();

	return EXIT_SUCCESS;
}
