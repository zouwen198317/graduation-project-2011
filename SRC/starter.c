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
#include <stdbool.h>
#include "internetConnector.h"
#include "serverCommunication.h"
/* End of Header files. */


/* Definitions. */
/* killProc() modes. */
#define KP_MODE				bool
#define	KP_BLOCK			0
#define	KP_NBLOCK			1
/* LOG definitions. */
#define LOGFILE				"Logfile"
/* Processes definitions. */
#define	PROC_NUM			4
#define PROC_GPS			0
#define PROC_CONNECTOR			1
#define PROC_COMM			2
/* End of Definitions. */


/* Macros. */
/* LOG macros. */
#define RAW_LOG( x, ...) 		log_write( x, __VA_ARGS__, NULL )
#define LOG( ... ) 			log_write( "Main parent", __VA_ARGS__, NULL )
/* End of Macros. */


/* Functions' prototypes. */
/* TODO: Inlining. */
inline int forkAndExec( int proc );
inline void cleanAndExit( int status );
inline void killProc( int procID, KP_MODE mode );
inline void killAll();
static void childDied( int signum, siginfo_t *siginfo, void *context );
char *procID2Name( int procID );
void execProc( int procID );
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
	LOG( "Initializing the log." );
	/* If log wasn't initialized successfully. */
	if( ret = log_init( LOGFILE ) )
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
	forkAndExec( PROC_CONNECTOR );
	forkAndExec( PROC_COMM );
	/* Forking GPS process. */
	forkAndExec( PROC_GPS );

	while( 1 )
		pause();

	LOG( "Parent process will sleep for 30 seconds." );
	sleep( 30 );
	LOG( "Parent woke up. Will terminate now." );

	cleanAndExit( EXIT_SUCCESS );
}

int forkAndExec( int procID )
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
		return EXIT_FAILURE;
	}
	if( !cpid )
	{
		/* Fork() succeeded, Child process. */
		/* Starting process. */
		RAW_LOG( "New Born child process", "Starting child process: ", procID2Name( procID ) ) ;
		execProc( procID );
		/* Process exited. */
		RAW_LOG( procID2Name( procID ), "W: Unexpected behaviour: Child process returned." );
		exit( EXIT_FAILURE );
	}

	/* Updating processesDB. */
	LOG( "Process forked successfully. Adding process to DB." );
	processesDB[ procID ] = cpid;
	processesCount++;
	return EXIT_SUCCESS;

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
				LOG( "Sending SIGTERM to process: ", procID2Name( i ) );
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
			LOG( "Dead child identified as: ", procID2Name( i ), ". Attempting to re-excute it." ) ;
			if( forkAndExec( i ) == EXIT_FAILURE )
			{
				LOG( "Failed to execute." );
				processesDB[ i ] = 0;
				processesCount--;
				LOG( "ProcessesDB updated." );
			}
			return;
		}
	LOG( "W: Failed to identify dead child. (PID = ", cpid, ")" );
}

void killProc( int procID, KP_MODE mode )
{
	/* Do nothing, if  the process isn't running. */
	while( processesDB[ procID ] )
	{
		/* Send SIGTERM to the process. */
		LOG( "Sending SIGTERM to process: ", procID2Name( procID ) );
		kill( processesDB[ procID ], SIGTERM );
		/* Return immediately, if the mode is set to non-block. */
		if( mode == KP_NBLOCK )
			return;
		/* Blocking mode:
		 * Setting an alarm in 2 seconds, so that if the parent process
		 * received SIGCHLD for any reason, it wouldn't hang.
		 */
		LOG( "Waiting for process ", procID2Name( procID ), " to terminate." );
		alarm( 2 );
		waitpid( processesDB[ procID ], NULL, 0 );
	}
}


char *procID2Name( int procID )
{
	switch( procID )
	{
		case PROC_GPS:
			return "GPS process";
		case PROC_CONNECTOR:
			return "Internet connector process";
		case PROC_COMM:
			return "Server communication process";
		default:
			return "Unkown Process";
	}
}


void execProc( int procID )
{
	switch( procID )
	{
		case PROC_GPS:
			gpsProcess();
			break;
		case PROC_CONNECTOR:
			inetConnect();
			break;
		case PROC_COMM:
			connectToServer();
			break;
		default:
			LOG( "Trying to execute an unkown process." );
	}
	return;
}
