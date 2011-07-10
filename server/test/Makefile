include ../../Common.mk
include ../SRC/Vars.mk
include Vars.mk


all: $(EXECUTABLES)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


%: test_%.o
	cd $(SRC_PATH); $(MAKE) $(MFLAGS) $($*_DEP)
	$(CC) $(CFLAGS) $(call uniq,$(call path_multiplier,$(SRC_PATH),$($*_DEP)) $(call path_multiplier,$(SRC_PATH),$(foreach var,$(patsubst %.o,%_DEP,$($*_DEP)),$($(var))))) test_$*.o -o $(BIN_PATH)$@


clean:
	$(RM) $(OBJECTS) $(call path_multiplier,$(EXTRA_PATH),$(EXTRA)) $(call path_multiplier,$(BIN_PATH),$(EXECUTABLES))
