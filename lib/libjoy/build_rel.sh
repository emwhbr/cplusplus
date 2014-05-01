#!/bin/bash
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

source ./setup_environment.sh

case "$1" in
    static)
	./build.sh release_static
        ;;

    shared)
	./build.sh release_shared
        ;;

    *)
        echo "Usage $0 {static|shared}"
        exit 1
        ;;
esac
