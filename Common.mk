SHELL := /bin/sh
CC := gcc
CFLAGS := -std=gnu99

ifeq "$(DEBUG)" "on"
CFLAGS += -g -Wall
endif

.PHONY: all clean

path_multiplier = `echo $(2) | sed 's/\ /\ $(1)/g' | sed 's/^/$(1)/g'`
