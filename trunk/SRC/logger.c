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

/* global variables. */
int	log_fd;


/* Functions. */
int log_init( char *fileName )
{
	if( fileName == NULL )
	{
		fileName = (char *) malloc( strlen(DEFAULT_LOG_FILE) );
		fileName = DEFAULT_LOG_FILE;
	}
	if( ( log_fd = open( fileName, O_WRONLY ) ) < 0 )
	{
		return errno;
	}



}

log_term()
{



}


log_write()
{



}
