# Makefile variables files for library codes
# Paths
BIN_PATH := ..\/bin\/
EXTRA_PATH := ..\/bin\/

# Global variables
OBJECTS := gpsParser.o gpsProcess.o logger.o starter.o sms.o serverCommunication.o internetConnector.o
EXECUTABLES := starter
EXTRA := Logfile

# Dependencies
starter_DEP := $(OBJECTS)
gpsProcess_DEP := logger.o gpsParser.o serverCommunication.o
sms_DEP := logger.o
serverCommunication_DEP := logger.o
internetConnector_DEP := logger.o
