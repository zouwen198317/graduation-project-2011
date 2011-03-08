include ../Common.mk
include Vars.mk


all: $(EXECUTABLES)


%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

starter.o: starter.c
	$(CC) $(CFLAGS) -c $< -o $@

%: %.o
	$(MAKE) $(MFLAGS) $($*_DEP)
	$(CC) $(CFLAGS) $($*_DEP) $*.o -o $(BIN_PATH)$@

%: %.c

clean:
	$(RM) $(OBJECTS) $(call path_multiplier,$(EXTRA_PATH),$(EXTRA)) $(call path_multiplier,$(BIN_PATH),$(EXECUTABLES))
