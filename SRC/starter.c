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
#include <errno.h>
/* End of Header files. */


/* Definitions. */
/* LOG definitions. */
#define LOGFILE				"Logfile"
/* Processes definitions. */
#define	PROC_NUM			2
#define PROC_GPS			0
/* End of Definitions. */


/* Macros. */
/* LOG macros. */
#define RAW_LOG( x, ...) 		log_write( x, __VA_ARGS__, NULL )
#define LOG( ... ) 			log_write( "Main parent", __VA_ARGS__, NULL )
/* procID translator macros. */
#define procID2Name( PROC_GPS )		"GPS Process"
/* processes translator macros. */
#define execProc( PROC_GPS )		gpsProcess()
/* End of Macros. */


/* Functions' prototypes. */
/* TODO: Inlining. */
inline void forkAndExec( int proc );
inline void cleanAndExit( int status );
inline void killAll();
static void childDied( int signum, siginfo_t *siginfo, void *context );
/* End of Functions' prototypes. */


/* Global variables. */
int processesDB[ PROC_NUM ];
int processesCount = 0;
/* End of Global variables. */


/* Functions. */
int main()
{
	int ret;
	struct sigaction sigHandle;


	/* Initializing the logger. */
	ret = log_init( LOGFILE );
	LOG( "Initializing the log." );
	/* If log wasn't initialized successfully. */
	if( ret )
		LOG( strerror( ret ) );

	/* Initializing processesDB. */
	memset( processesDB, 0, sizeof( int ) * PROC_NUM );

	LOG( "Masking signals." );
	/* Masking signals. */
	/* SIGCHLD. */
	sigHandle.sa_sigaction = &childDied;
	sigemptyset( &sigHandle.sa_mask );
	sigHandle.sa_flags = SA_SIGINFO;
	if( ( sigaction( SIGCHLD, &sigHandle, NULL ) ) < 0 )
		LOG( "W: Failed to mask SIGCHLD. Expecting messy behaviour." );
	else
		LOG( "SIGCHLD mask set." );


	/* Starting the forking phase. */
	/* Forking GPS process. */
	forkAndExec( PROC_GPS );

	LOG( "Parent process will sleep for 30 seconds." );
	sleep( 30 );
	LOG( "Parent woke up. Will terminate now." );
//	waitpid( pid_gps, NULL, 0 );

	cleanAndExit( EXIT_SUCCESS );
}

void forkAndExec( int procID )
{
	int ret_err;
	pid_t cpid = fork();
	if( cpid < 0 )
	{
		/* Error in forking. Logging and exiting. */
		ret_err = errno;
		LOG( "Process failed to fork." );
		LOG( strerror( ret_err ) );
		LOG( "Fatal error, quiting." );
		cleanAndExit( EXIT_FAILURE );
	}
	if( !cpid )
	{
		/* Fork() succeeded, Child process. */
		/* Starting process. */
		RAW_LOG( "New Born child process", "Starting child process: ", procID2Name( procID ) ) ;
		execProc( procID );
		/* Process exited. */
		/* TODO: not supposed to get back here. */
		RAW_LOG( procID2Name( procID ), "W: Unexpected behaviour: Child process returned." );
		exit( EXIT_SUCCESS );
	}

	/* Updating processesDB. */
	LOG( "Process forked successfully. Adding process to DB." );
	processesDB[ procID ] = cpid;
	processesCount++;
printf("(%d) %d:%s -> %d\n", processesCount, procID, procID2Name( procID ), cpid );

}

void cleanAndExit( int status )
{
	/* Killing children. */
	LOG( "Killing running processes." );
	killAll();
	/* Terminating log. */
	LOG( "Terminating log and exiting." );
	log_term();
	/* Exiting. */
	exit( status );
}

void killAll()
{
	/* Sending SIGTERM to all active processes. */
	while( processesCount )
	{
		for( int i = 0; i < PROC_NUM; i++ )
			if( processesDB[ i ] )
			{
				LOG( "Sending SIGTERM to process ", procID2Name( i ) );
				kill( processesDB[ i ], SIGTERM );
			}
		sleep(1);
	}
}

static void childDied( int signum, siginfo_t *siginfo, void *context )
{
	LOG( "Caught SIGCHLD." );
	/* Identifying dead child. */
	pid_t cpid = siginfo->si_pid;
	for( int i = 0; i < PROC_NUM; i++ )
		if( processesDB[ i ] == cpid )
		{
			/* Process identified, updating DB. */
			LOG( "Dead child identified as: ", procID2Name( i ) ) ;
			processesDB[ i ] = 0;
			processesCount--;
			LOG( "ProcessesDB updated." );
			return;
			break;
		}
	LOG( "W: Failed to identify dead child." );
}

