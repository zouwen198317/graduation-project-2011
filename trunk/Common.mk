CC := gcc
CFLAGS := -std=gnu99

ifeq "$(DEBUG)" "on"
CFLAGS += -g -Wall
endif
