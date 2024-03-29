include Common.mk

all:
	cd SRC/ ; $(MAKE) $(MFLAGS)

test_%:
	cd test/ ; $(MAKE) $(MFLAGS) $*

server_%:
	cd server/ ; $(MAKE) $(MFLAGS) $*

%:
	cd SRC/; $(MAKE) $(MFLAGS) $*

clean: test_clean server_clean
	cd SRC/ ; $(MAKE) $(MFLAGS) clean
