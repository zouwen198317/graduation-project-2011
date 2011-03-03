include ../Common.mk
include Vars.mk

path_multiplier = `echo $(2) | sed 's/\ /\ $(1)/g' | sed 's/^/$(1)/g'`

all: $(EXECUTABLES)


%.o : %.c
	$(CC) $(CFLAGS) -c $^ -o $@


%: test_%.o
	cd ../SRC/; make $($@_DEP)
	$(CC) $(CFLAGS) $(call path_multiplier,..\/SRC\/,$($@_DEP)) test_$@.o -o ../test_bin/$@


clean:
	rm -rf $(OBJECTS) $(call path_multiplier,..\/test_bin\/extra\/,$(EXTRA)) $(call path_multiplier,..\/test_bin\/,$(EXECUTABLES))
