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

# Path to toolchain
export PATH=:$PATH
#export PATH=/opt/rpi/toolchains/x-tools/arm-unknown-linux-gnueabihf/bin:$PATH

# Toolchain specific
export ARCH=x86_64
#export ARCH=arm

export CFLAGS_ARCH_TUNING=-m64
#export CFLAGS_ARCH_TUNING="-march=armv6 -mfpu=vfp -mfloat-abi=hard"

export TC_PREFIX=
#export TC_PREFIX=arm-unknown-linux-gnueabihf-
