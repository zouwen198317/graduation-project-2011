include Common.mk


all:
	cd SRC/ ; make

test_%:
	cd test/ ; make $*

clean: test_clean
	cd SRC/ ; make clean
