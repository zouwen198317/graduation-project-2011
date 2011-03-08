include ../Common.mk
include Vars.mk


all: $(EXECUTABLES)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


%: test_%.o
	cd $(SRC_PATH); $(MAKE) $(MFLAGS) $($*_DEP)
	$(CC) $(CFLAGS) $(call path_multiplier,$(SRC_PATH),$($*_DEP)) test_$*.o -o $(BIN_PATH)$@


clean:
	$(RM) $(OBJECTS) $(call path_multiplier,$(EXTRA_PATH),$(EXTRA)) $(call path_multiplier,$(BIN_PATH),$(EXECUTABLES))
