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

################################################################
function get_parallel_args()
################################################################
{
    # Check number of CPU's in this machine
    nr_cpus=`cat /proc/cpuinfo | grep processor | wc -l`
    
    # Add one to get number of parallel jobs
    ((nr_jobs=nr_cpus + 1))
    
    echo "-j${nr_jobs}"
    return 0
}

### Number of parallel jobs on this machine
PARALLEL_ARGS=`get_parallel_args`

case "$1" in
    release_static)
        echo "==[MAKE RELEASE - STATIC]==="
        make JOBS=${PARALLEL_ARGS} BUILD_TYPE=RELEASE LIB_TYPE=STATIC all
        ;;

    release_shared)
        echo "==[MAKE RELEASE - SHARED]==="
        make JOBS=${PARALLEL_ARGS} BUILD_TYPE=RELEASE LIB_TYPE=SHARED all
        ;;

    debug_static)
        echo "==[MAKE DEBUG - STATIC]==="
        make JOBS=${PARALLEL_ARGS} BUILD_TYPE=DEBUG LIB_TYPE=STATIC all
        ;;

    debug_shared)
        echo "==[MAKE DEBUG - SHARED]==="
        make JOBS=${PARALLEL_ARGS} BUILD_TYPE=DEBUG LIB_TYPE=SHARED all
        ;;

    clean)
        echo "==[CLEANUP]==="
        make clean
        ;;

    *)
        echo "Usage $0 {release_static|release_shared|debug_static|debug_shared|clean}"
        exit 1
        ;;
esac
