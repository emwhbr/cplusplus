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

include common_defs.mk

# ------ Targets

.PHONY : lib test_lib all clean

lib :
	cd ./src ; make $(JOBS) lib

test_lib :
	cd ./test ; make $(JOBS) test

all : lib test_lib

clean :
	rm -f *~
	cd ./src ; make lib_clean
	cd ./test ; make test_clean
