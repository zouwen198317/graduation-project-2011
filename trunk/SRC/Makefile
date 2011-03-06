include ../Common.mk
include Vars.mk


all: $(EXECUTABLES)


%.o: %.c %.h
	${CC} ${CFLAGS} -c $< -o $@


%: %.o $(%_DEP)
	$(CC) $(CFLAGS) $($*_DEP) $*.o -o $(BIN_PATH)$@

%: %.c

clean:
	rm -rf $(OBJECTS) $(call path_multiplier,$(EXTRA_PATH),$(EXTRA)) $(call path_multiplier,$(BIN_PATH),$(EXECUTABLES))
