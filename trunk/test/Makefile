CC := gcc
CFLAGS := -std=gnu99


all: gpsParser


gpsParser: test_gps_parser.o ../SRC/gps_parser.o
	${CC} $^ -o $@

