/*
 * The logger testing code.
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
#include "../SRC/logger.h"

/* Definitions. */
#define LOGFILENAME	"extra/logfile"

/* Marcros. */
#define LOG( x )		log_write( "test_log", x );

/* Functions. */
int main()
{
	int ret;

	LOG( "Pre-initialization logging." );
	if( ( ret = log_init( LOGFILENAME ) ) )
	{
		printf("Error opening log file: %s\n", strerror( ret ) );
		exit( EXIT_FAILURE );
	}
	
	printf("Log file opened successfully.\n");
	LOG( "First log." );
	LOG( "Second log." );
	printf("Logs written.\n");
	LOG( "Closing log." );

	if( ( ret = log_term() ) )
	{
		LOG( "Cannot close log." );
		printf( "Error closing log: %s\n", strerror( ret ) );
	}
	printf( "Log file closed successfully.\n");
	LOG( "Post-initialization logging." );
	return 0;
}

