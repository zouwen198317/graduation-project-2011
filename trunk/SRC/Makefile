include ../Common.mk
include Vars.mk


all: $(OBJECTS)

%.o : %.c %.h
	${CC} ${CFLAGS} -c $< -o $@

clean:
	rm -rf $(OBJECTS)
