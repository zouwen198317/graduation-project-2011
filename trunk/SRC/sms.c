/* Header files. */
#include "sms.h"
#include "logger.h"

/* Definitions. */
#define BAUDRATE	B115200
#define MDMDEV		"/dev/ttyUSB0"
#define LOG( ... )	log_write( "SMS", __VA_ARGS__, NULL )

/* Function prototypes. */
bool _sms_AT( int, char *, char * );

/* Functions. */
int send_sms( char *number, char *message_body )
{

	int			modem_fd = 0;
	struct termios		oldTerm;
	struct termios		newTerm;


	LOG( "Opening file \'", MDMDEV, "\' for reading and writing." );
	while( ( modem_fd = open( MDMDEV, O_RDWR | O_NOCTTY ) ) < 0 )
	{
		/* Error opening the device file. */
		LOG( strerror( errno ) ) ;
		/* TODO:
		 * Report error to display
		 */
		sleep(1);
	}

	/* Storing old terminal settings. */
	tcgetattr( modem_fd, &oldTerm );
	/* Defining the serial terminal setups:
	 * using canonical synchronous mode.
	 */
	bzero( &newTerm, sizeof( newTerm ) );
	newTerm.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newTerm.c_iflag = IGNPAR;
	newTerm.c_oflag = 0;
	newTerm.c_lflag = ICANON;
	/* Flush any garbage in the device. */
	tcflush( modem_fd, TCIFLUSH );
	/* Apply the new terminal settings. */
	tcsetattr( modem_fd, TCSANOW, &newTerm );
	LOG( "Serial terminal is configured. Starting to send AT commands." );

	while( !_sms_AT( modem_fd, number, message_body ) )
	{
		LOG( "Error encountered while sending message. Retrying in one second." );
		sleep( 1 );
	}

	close( modem_fd );
	return 0;
}

bool _sms_AT( int modem_fd, char * number, char * msg )
{

	register int		count;
	char			buff[ 255 ];
	bool			flag;

	/* Sending AT commands to USB modem. */
	/*TODO*/

	flag = true;
	while( flag )
	{
		LOG( "AT command: sending \"ATZ\"." );
		if ( ( count = write( modem_fd, "AT\r", 4 ) ) != 4 )
		{
			LOG( strerror( errno ) );
			continue;
		}
		sleep( 2 );
		count = read( modem_fd, &buff, 255 );
		buff[ count ] = '\0';
		LOG( "AT command: received \"", buff, "\"." );
		if( strncmp( buff, "OK", 2 ) == 0 )
			flag = false;
	}


	flag = true;
	while( flag )
	{
		LOG( "AT command: sending \"AT+CMGF=1\"." );
		if ( ( count = write( modem_fd, "AT+CMGF=1\r", 11 ) ) != 11 )
		{
			LOG( strerror( errno ) );
			continue;
		}
		sleep( 2 );
		count = read( modem_fd, &buff, 255 );
		buff[ count ] = '\0';
		LOG( "AT command: received \"", buff, "\"." );
		if( strncmp( buff, "OK", 2 ) == 0 )
			flag = false;
	}


	flag = true;
	while( flag )
	{
		LOG( "Sending the phone number.");
		sprintf( buff, "AT+CMGW=\"%s\"\r", number );
		LOG( "AT command: sending \"", buff, "\"." );
		if ( ( count = write( modem_fd, buff, 5 ) ) != 5 )
		{
			LOG( strerror( errno ) );
			continue;
		}
		sleep( 2 );
		count = read( modem_fd, &buff, 255 );
		buff[ count ] = '\0';
		LOG( "AT command: received \"", buff, "\"." );
		if( strncmp( buff, ">", 1 ) == 0 )
			flag = false;
	}


	flag = true;
	while( flag )
	{
		LOG( "Sending the message body.");
		LOG( "AT command: sending \"", msg, "\"." );
		if ( ( count = write( modem_fd, msg, sizeof(msg) ) ) != sizeof(msg) )
		{
			LOG( strerror( errno ) );
			continue;
		}
		LOG( "Sending CTR-Z." );
		buff[ 0 ] = 26;
		buff[ 1 ] = '\r';
		buff[ 2 ] = '\0';
		if ( ( count = write( modem_fd, buff, 2 ) ) != 2 )
		{
			LOG( strerror( errno ) );
			continue;
		}
		sleep( 2 );
		count = read( modem_fd, &buff, 255 );
		buff[ count ] = '\0';
		LOG( "AT command: received \"", buff, "\"." );
		if( strncmp( buff, "OK", 2 ) == 0 )
			flag = false;
	}





	return 1;
}
