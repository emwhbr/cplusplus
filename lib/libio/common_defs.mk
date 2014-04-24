# /************************************************************************
#  *                                                                      *
#  * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
#  *                                                                      *
#  * This program is free software; you can redistribute it and/or modify *
#  * it under the terms of the GNU General Public License as published by *
#  * the Free Software Foundation; either version 2 of the License, or    *
#  * (at your option) any later version.                                  *
#  *                                                                      *
#  ************************************************************************/

# ----- Toolchain setup

ARCH ?= unknown
CFLAGS_ARCH_TUNING ?= -march=unknown-cpu
TC_PREFIX ?= unknown-linux-

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

OBJ_DIR = ../obj
INC_DIR = ../src

LIB_NAME = io
LIB_BASENAME = $(OBJ_DIR)/lib${LIB_NAME}
ifeq "$(LIB_TYPE)" "STATIC"
	LIB_FILE_NAME = $(LIB_BASENAME).a
else
	LIB_FILE_NAME = $(LIB_BASENAME).so
endif

# ----- Compiler flags

CFLAGS = -Wall -Wextra -Werror -Dlinux -Wno-packed-bitfield-compat
CFLAGS += $(CFLAGS_ARCH_TUNING)
CFLAGS += $(OPTIMIZE)
CFLAGS += $(DEBUG_PRINTS)

COMP_FLAGS = $(CFLAGS) -c
COMP_FLAGS_C   = $(COMP_FLAGS) -std=c99
COMP_FLAGS_CPP = $(COMP_FLAGS) -std=c++98 -pedantic -Wno-long-long -Wno-variadic-macros

# ----- Compiler includes

LIB_INCLUDE = -I$(INC_DIR)
INCLUDES = $(LIB_INCLUDE)
