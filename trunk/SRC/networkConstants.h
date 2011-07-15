/*
 * The network constants header file.
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

#ifndef __NETWORK_CONSTANTS__
#define __NETWORK_CONSTANTS__


/* Definitions. */
#define IDENTIFICATION_PATTERN	"#3c4r1d3n74ndr3d1rm3@"
#define INITPORT		6543
#define NETWORK_BUFFER_SIZE	4096
#define GPS_IDENT		1

/* Macros. */
#define _getsize( buffer )	(uint32_t)( (uint8_t)(buffer[3]) + 8 * (uint8_t)(buffer[2]) + 64 * (uint8_t)(buffer[1])  + 512 * (uint8_t)(buffer[0]) )


#endif
