include Common.mk

all:
	cd SRC/ ; $(MAKE) $(MFLAGS)

test_%:
	cd test/ ; $(MAKE) $(MFLAGS) $*

clean: test_clean
	cd SRC/ ; $(MAKE) $(MFLAGS) clean
