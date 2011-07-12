# Makefile variables files for library codes
# Paths
BIN_PATH := ..\/bin\/
EXTRA_PATH := ..\/bin\/

# Global variables
OBJECTS := logger.o gpsXml.o commServerD.o
EXECUTABLES := commServerD
EXTRA := Logfile

# Dependencies
commServerD_DEP := $(OBJECTS)
gpsXml_DEP := logger.o
