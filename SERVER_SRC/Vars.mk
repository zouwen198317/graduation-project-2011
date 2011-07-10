# Makefile variables files for library codes
# Paths
BIN_PATH := ..\/server_bin\/
EXTRA_PATH := ..\/server_bin\/

# Global variables
OBJECTS := logger.o gpsXml.o carsCommunicationDaemon.o
EXECUTABLES := carsCommunicationDaemon
EXTRA := Logfile

# Dependencies
carsCommunicationDaemon_DEP := $(OBJECTS)
gpsXml_DEP := logger.o
