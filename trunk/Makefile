CC := gcc
CFLAGS := -std=gnu99

all: gps_parser.o test_gps_parser


test_gps_parser: test_gps_parser.o gps_parser.o
	${CC} $^ -o $@

test_gps_parser.o: test_gps_parser.c
	${CC} ${CFLAGS} -c $< -o $@

gps_parser.o: gps_parser.c
	${CC} ${CFLAGS} -c $< -o $@


clean:
	rm -rf *.o test_gps_parser
