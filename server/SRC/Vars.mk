# Makefile variables files for library codes
# Paths
BIN_PATH := ..\/bin\/
EXTRA_PATH := ..\/bin\/

# Global variables
OBJECTS := logger.o gpsXml.o carsCommunicationDaemon.o
EXECUTABLES := carsCommunicationDaemon
EXTRA := Logfile

# Dependencies
carsCommunicationDaemon_DEP := $(OBJECTS)
gpsXml_DEP := logger.o
