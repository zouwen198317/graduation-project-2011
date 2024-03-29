/*
 * The GPS data grabber header file.
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

#ifndef __GPSPROCESS_H_
#define __GPSPROCESS_H_

/* Header files. */
#include <termios.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "logger.h"
#include <unistd.h>
#include "gpsParser.h"
#include <signal.h>

/* Functions' prototypes. */
/* gpsProcess:
 * This is the function that does the gps process task; it opens the gps device
 * file, starts reading data, parses the data, and send them to the display (if
 * found) and to the server over the network.
 */
void gpsProcess();


#endif
