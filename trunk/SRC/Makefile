include ../Common.mk
include Vars.mk


all: $(EXECUTABLES)


%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@


%: %.c
	if [ "$(strip $($*_DEP))" != "" ]; then $(MAKE) $(MFLAGS) $($*_DEP); fi
	$(CC) $(CFLAGS) $($*_DEP) -o $(BIN_PATH)$@


clean:
	$(RM) $(OBJECTS) $(call path_multiplier,$(EXTRA_PATH),$(EXTRA)) $(call path_multiplier,$(BIN_PATH),$(EXECUTABLES))
