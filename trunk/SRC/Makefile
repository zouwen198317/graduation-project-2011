include ../Common.mk
include Vars.mk


all: $(EXECUTABLES)


%.o: %.c 
	${CC} ${CFLAGS} -c $< -o $@


%: %.o $(%_DEP)
	$(CC) $(CFLAGS) $($@_DEP) $@.o -o $(BIN_PATH)$@


clean:
	rm -rf $(OBJECTS) $(call path_multiplier,$(EXTRA_PATH),$(EXTRA)) $(call path_multiplier,$(BIN_PATH),$(EXECUTABLES))
