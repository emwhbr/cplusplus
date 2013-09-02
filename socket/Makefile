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

TEST_OBJS1 = $(OBJ_DIR)/socket_support.o \
	     $(OBJ_DIR)/test_socket.o

TEST_OBJS2 = $(OBJ_DIR)/socket_support.o \
	     $(OBJ_DIR)/test_tcp_socket.o

TEST_OBJS3 = $(OBJ_DIR)/socket_support.o \
	     $(OBJ_DIR)/test_udp_socket.o

TEST_APP_NAME1 = $(OBJ_DIR)/test_socket_$(KIND).$(ARCH)
TEST_APP_NAME2 = $(OBJ_DIR)/test_tcp_socket_$(KIND).$(ARCH)
TEST_APP_NAME3 = $(OBJ_DIR)/test_udp_socket_$(KIND).$(ARCH)

# ----- Compiler flags

CFLAGS = -Wall -Werror
CFLAGS += $(OPTIMIZE)
CFLAGS += $(DEBUG_PRINTS)

LINK_FLAGS = $(CFLAGS)
COMP_FLAGS = $(LINK_FLAGS) -c

# ----- Includes

TEST_INCLUDE  = -I$(INC_DIR)

INCLUDE = $(TEST_INCLUDE)

# ----- Linker libraries

LIBSX = -lstdc++ -lgcc -lpthread -lrt
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

test_app1 : $(TEST_OBJS1)
	$(CC) $(LINK_FLAGS) -o $(TEST_APP_NAME1) $(TEST_OBJS1) $(LIBS)

test_app2 : $(TEST_OBJS2)
	$(CC) $(LINK_FLAGS) -o $(TEST_APP_NAME2) $(TEST_OBJS2) $(LIBS)

test_app3 : $(TEST_OBJS3)
	$(CC) $(LINK_FLAGS) -o $(TEST_APP_NAME3) $(TEST_OBJS3) $(LIBS)

all : test_app1 test_app2 test_app3

clean :
	rm -f $(TEST_OBJS1) $(TEST_OBJS2) $(TEST_OBJS3) $(OBJ_DIR)/*.$(ARCH) *~

help:
	@echo "Usage: make clean"
	@echo "       make test_app1"
	@echo "       make test_app2"
	@echo "       make test_app3"
