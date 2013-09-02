# /***********************************************************************
# *                                                                      *
# * Copyright (C) 2013 Bonden i Nol (hakanbrolin@hotmail.com)            *
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

TEST_OBJS1 = $(OBJ_DIR)/thread.o \
             $(OBJ_DIR)/worker_thread.o \
	     $(OBJ_DIR)/test_thread.o

TEST_OBJS2 = $(OBJ_DIR)/thread.o \
             $(OBJ_DIR)/cyclic_thread.o \
             $(OBJ_DIR)/worker_cyclic_thread.o \
             $(OBJ_DIR)/test_cyclic_thread.o

COMMON_CODE_DIR = ..

DELAY_DIR = $(COMMON_CODE_DIR)/delay
TIMER_DIR = $(COMMON_CODE_DIR)/timer

DELAY_OBJ = $(OBJ_DIR)/delay.o
TIMER_OBJ = $(OBJ_DIR)/timer.o

TEST_APP_NAME1 = $(OBJ_DIR)/test_thread_$(KIND).$(ARCH)
TEST_APP_NAME2 = $(OBJ_DIR)/test_cyclic_thread_$(KIND).$(ARCH)

# ----- Compiler flags

CFLAGS = -Wall
CFLAGS += $(OPTIMIZE)
CFLAGS += $(DEBUG_PRINTS)

LINK_FLAGS = $(CFLAGS)
COMP_FLAGS = $(LINK_FLAGS) -c

# ----- Includes

TEST_INCLUDE  = -I$(INC_DIR) -I$(DELAY_DIR) -I$(TIMER_DIR)
DELAY_INCLUDE = -I$(DELAY_DIR)
TIMER_INCLUDE = -I$(TIMER_DIR)

INCLUDE = $(TEST_INCLUDE)

# ----- Linker libraries

LIBSX = -lpthread -lrt -lstdc++
LIBS  = $(LIBSX)

# ------ Build rules

.SUFFIXES:
.SUFFIXES: .c .cpp .o .h

$(OBJ_DIR)/%.o : %.c
	$(CC) $(COMP_FLAGS) $(INCLUDE) -o $@ $<

$(OBJ_DIR)/%.o : %.cpp
	$(CPP) $(COMP_FLAGS) $(INCLUDE) -o $@ $<

$(DELAY_OBJ) : $(DELAY_DIR)/delay.cpp
	$(CPP) $(COMP_FLAGS) $(DELAY_INCLUDE) -o $@ $(DELAY_DIR)/delay.cpp

$(TIMER_OBJ) : $(TIMER_DIR)/timer.cpp
	$(CPP) $(COMP_FLAGS) $(DELAY_INCLUDE) -o $@ $(TIMER_DIR)/timer.cpp

# ------ Targets

.PHONY : clean help

test_app1 : $(TEST_OBJS1) $(DELAY_OBJ)
	$(CC) $(LINK_FLAGS) -o $(TEST_APP_NAME1) $(TEST_OBJS1) $(DELAY_OBJ) $(LIBS)

test_app2 : $(TEST_OBJS2) $(DELAY_OBJ) $(TIMER_OBJ)
	$(CC) $(LINK_FLAGS) -o $(TEST_APP_NAME2) $(TEST_OBJS2) $(DELAY_OBJ) $(TIMER_OBJ) $(LIBS)

clean :
	rm -f $(TEST_OBJS1) $(TEST_OBJS2) $(DELAY_OBJ) $(TIMER_OBJ) $(OBJ_DIR)/*.$(ARCH) *~

help:
	@echo "Usage: make clean"
	@echo "       make test_app1"
	@echo "       make test_app2"
