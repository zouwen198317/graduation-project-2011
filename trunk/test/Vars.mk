# Makefile variables file for test codes

# Global Variables
OBJECTS := test_gpsParser.o test_gpsProcess.o
EXECUTABLES := gpsParser gpsProcess
EXTRA := gps_sample.out

# Dependencies
gpsParser_DEP := gpsParser.o
gpsProcess_DEP := gpsProcess.o
