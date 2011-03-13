/*
 * The GPS data grabber Code file.
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

/* TODO: remove: */
//#include<stdio.h>


/* Definitions. */
#define BAUDRATE	B9600
#define GPSDEV		"/dev/ttyUSB0"


/* Marcros. */
#define LOG( x )	log_write( "GPS Process", x )

/* Functions. */

void gpsProcess()
{
	register int gps_fd, count;
	struct termios oldTerm, newTerm;
	char buff[ 255 ];

	/* Opening GPS device (serial terminal) for reading. */
	while( ( gps_fd = open( GPSDEV, O_RDONLY | O_NOCTTY ) ) < 0 )
	{
		/* Error opening the device file. */
		LOG( strerror( errno ) );
		/* TODO:
		 * Report error to display
		 */
		/* TODO:
		 * Interrupt instead of poll
		 */
		sleep(1);
	}

	/* TODO:
	 * Mask term signal.
	 */


	/* Storing old terminal settings. */
	tcgetattr( gps_fd, &oldTerm );
	/* Defining the serial terminal setups:
	 * using canonical synchronous mode.
	 */
	bzero( &newTerm, sizeof( newTerm ) );
	newTerm.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newTerm.c_iflag = IGNPAR;
	newTerm.c_oflag = 0;
	newTerm.c_lflag = ICANON;
	/* Flush any garbage in the device. */
	tcflush( gps_fd, TCIFLUSH );
	/* Apply the new terminal settings. */
	tcsetattr( gps_fd, TCSANOW, &newTerm );

	/* Data fetching loop. */
	while(1)
	{
		GPSData parsedData;

		/* Reading data from device. */
		count = read( gps_fd, buff, 255 );
		buff[ count ] = '\0';
//		printf( "%s\n", buff );
		/* Parsing the data fetched. */
		if( NMEARead( buff, &parsedData ) )
		{


		}
	}

	/* TODO:
	 * The cleanup code should be in an interupt handler.
	 */
	tcsetattr( gps_fd, TCSANOW, &oldTerm );

}

