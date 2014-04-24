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

include ../common_defs.mk

OBJS = $(OBJ_DIR)/io.o \
	$(OBJ_DIR)/io_core.o \
	$(OBJ_DIR)/io_exception.o \
	$(OBJ_DIR)/io_utility.o \
        $(OBJ_DIR)/io_gpio.o

ifeq "$(LIB_TYPE)" "STATIC"
	COMP_FLAGS_C_LIB   = $(COMP_FLAGS_C)
	COMP_FLAGS_CPP_LIB = $(COMP_FLAGS_CPP)
else
	COMP_FLAGS_C_LIB   = $(COMP_FLAGS_C) -fPIC
	COMP_FLAGS_CPP_LIB = $(COMP_FLAGS_CPP) -fPIC
endif

# ------ Build rules

.SUFFIXES:
.SUFFIXES: .c .cpp .o .h .d

$(OBJ_DIR)/%.o : %.c
	$(CC) $(COMP_FLAGS_C_LIB) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/%.d : %.c
	@$(CC) -MM -MT '$(patsubst %d,%o,$@)' $(COMP_FLAGS_C_LIB) $(INCLUDES) $< > $@

$(OBJ_DIR)/%.o : %.cpp
	$(CPP) $(COMP_FLAGS_CPP_LIB) $(INCLUDES) -o $@ $<

$(OBJ_DIR)/%.d : %.cpp
	@$(CPP) -MM -MT '$(patsubst %d,%o,$@)' $(COMP_FLAGS_CPP_LIB) $(INCLUDES) $< > $@

# ----- Targets

.PHONY : lib_clean

-include $(OBJS:.o=.d)

lib : $(OBJS)
ifeq "$(LIB_TYPE)" "STATIC"
	$(AR) rcs $(LIB_FILE_NAME) $(OBJS)
else
	$(CPP) -shared -o $(LIB_FILE_NAME) $(OBJS)
endif

lib_clean :
	rm -f $(OBJS) $(OBJS:.o=.d) $(LIB_BASENAME)* *~
