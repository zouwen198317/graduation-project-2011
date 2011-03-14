/*
 * The logger header file.
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

#ifndef __LOGGER_H_
#define __LOGGER_H_

/* Header files. */
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <alloca.h>

/* Functions' prototypes. */
/* log_init:
 * This function is reponsible for initializing the logger. It takes only one
 * argument, LogFileName, which should be the file (path + name) to be used as
 * the logging file. LogFileName can be either be an absolute path or a relative
 * one.
 * If the file failed to open for any reason, the function returns the errno
 * produced by the function tha failed.
 * If log_init was called again after it has succeeded once, it does nothing and
 * returns EMFILE (Error: Too many open files.).
 */
int log_init( char *LogFileName );

/* log_term:
 * This function is responsible for terminating the logger, i.e., closes the log
 * file and do any other work when necessary. It takes no arguments.
 * This function also returns errno, in case of failure.
 * If log_term was called when no log file were initialized it does nothing and
 * returns ENOTSUP (Error: Operation not supported.).
 */
int log_term();

/* log_write:
 * This is the logging function. It has two parameters; The first is a pointer
 * to character that points to message title, and the second is a pointer to
 * character that points to the message text.
 * If log_write was called without the logger being initialized, it prints its
 * log message to FILENO_STDERR.
 */
int log_write( char *title, char *msg );


#endif
