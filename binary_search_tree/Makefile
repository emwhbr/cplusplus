# /***********************************************************************
# *                                                                      *
# * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
# *                                                                      *
# * This program is free software; you can redistribute it and/or modify *
# * it under the terms of the GNU General Public License as published by *
# * the Free Software Foundation; either version 2 of the License, or    *
# * (at your option) any later version.                                  *
# *                                                                      *
# ************************************************************************/

# ----- Toolchain setup

ARCH ?= x86_64

TC_PREFIX ?=

AR  = $(TC_PREFIX)ar
CC  = $(TC_PREFIX)gcc
CPP = $(TC_PREFIX)g++
AS  = $(TC_PREFIX)gcc
LD  = $(TC_PREFIX)gcc

# ----- Naive setup

ifeq "$(BUILD_TYPE)" "RELEASE"
	OPTIMIZE = -O3
	KIND = rel
else 
	OPTIMIZE = -O0 -g3
	KIND = dbg
	DEBUG_PRINTS = -DDEBUG_PRINTS
endif

OBJ_DIR = ./obj
INC_DIR = ./

TEST_OBJS = $(OBJ_DIR)/bst.o \
	    $(OBJ_DIR)/test_bst.o

TEST_APP_NAME = $(OBJ_DIR)/test_bst_$(KIND).$(ARCH)

# ----- Compiler flags

CFLAGS = -Wall
CFLAGS += $(OPTIMIZE)
CFLAGS += $(DEBUG_PRINTS)

LINK_FLAGS = $(CFLAGS)
COMP_FLAGS = $(LINK_FLAGS) -c

# ----- Includes

TEST_INCLUDE  = -I $(INC_DIR)

INCLUDE = $(TEST_INCLUDE)

# ----- Linker libraries

LIBSX = -lrt -lstdc++
LIBS  = $(LIBSX)

# ------ Build rules

.SUFFIXES:
.SUFFIXES: .c .cpp .o .h

$(OBJ_DIR)/%.o : %.c
	$(CC) $(COMP_FLAGS) $(INCLUDE) -o $@ $<

$(OBJ_DIR)/%.o : %.cpp
	$(CPP) $(COMP_FLAGS) $(INCLUDE) -o $@ $<

# ------ Targets

.PHONY : clean help

test_app : $(TEST_OBJS)
	$(CC) $(LINK_FLAGS) -o $(TEST_APP_NAME) $(TEST_OBJS) $(LIBS)

all : test_app

clean :
	rm -f $(TEST_OBJS) $(OBJ_DIR)/*.$(ARCH) *~

help:
	@echo "Usage: make clean"
	@echo "       make test_app"
