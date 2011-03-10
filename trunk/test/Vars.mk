# Makefile variables file for test codes
# Paths
BIN_PATH := ..\/test_bin\/
EXTRA_PATH := ..\/test_bin\/extra\/
SRC_PATH := ..\/SRC\/

# Global Variables
OBJECTS := test_gpsParser.o test_gpsProcess.o test_logger.o
EXECUTABLES := gpsParser gpsProcess logger
EXTRA := gps_sample.out logfile gpsProcessTestLog

# Dependencies
gpsParser_DEP += gpsParser.o
gpsProcess_DEP += gpsProcess.o logger.o
logger_DEP += logger.o
