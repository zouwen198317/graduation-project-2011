include ../Common.mk
include Vars.mk

all: $(EXECUTABLES)


%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@


%: test_%.o
	cd ../SRC/; make $($@_DEP)
	$(CC) $(CFLAGS) ../SRC/$($@_DEP) test_$@.o -o ../test_bin/$@


clean:
	rm -rf $(OBJECTS) ../SAMPLES/$(EXTRA) ../test_bin/*
