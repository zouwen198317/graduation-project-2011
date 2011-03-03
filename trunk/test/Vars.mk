# Makefile variables file for test codes

# Global Variables
OBJECTS := test_gpsParser.o test_gpsProcess.o test_logger.o
EXECUTABLES := gpsParser gpsProcess logger
EXTRA := gps_sample.out

# Dependencies
gpsParser_DEP := gpsParser.o
gpsProcess_DEP := gpsProcess.o
logger_DEP := logger.o
