SHELL := /bin/sh
CC := gcc
CFLAGS := -std=gnu99 `mysql_config --cflags` -L/usr/lib/mysql -lmysqlclient

ifeq "$(DEBUG)" "on"
	CFLAGS += -g -Wall -Winline
endif

.PHONY: all clean

#Functions
path_multiplier = `echo $(2) | sed 's/\ /\ $(1)/g' | sed 's/^/$(1)/g'`
uniq = $(shell echo $(1) | sed 's/\ /\n/g' | sort -u | tr '\n' ' ')
