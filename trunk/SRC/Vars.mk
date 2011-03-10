# Makefile variables files for library codes
# Paths
BIN_PATH := ..\/bin\/
EXTRA_PATH := ..\/bin\/

# Global variables
OBJECTS := gpsParser.o gpsProcess.o logger.o
EXECUTABLES := starter
EXTRA := Logfile

# Dependencies
starter_DEP := $(OBJECTS)
gpsProcess_DEP := logger.o gpsParser.o
