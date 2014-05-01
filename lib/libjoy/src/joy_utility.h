// ************************************************************************
// *                                                                      *
// * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
// *                                                                      *
// * This program is free software; you can redistribute it and/or modify *
// * it under the terms of the GNU General Public License as published by *
// * the Free Software Foundation; either version 2 of the License, or    *
// * (at your option) any later version.                                  *
// *                                                                      *
// ************************************************************************

#ifndef __JOY_UTILITY_H__
#define __JOY_UTILITY_H__

#include <pthread.h>

using namespace std;

extern long joy_do_mutex_lock(pthread_mutex_t *mutex);

extern long joy_do_mutex_unlock(pthread_mutex_t *mutex);

extern long joy_get_my_pid(void);

extern long joy_get_my_thread_id(void);

extern long joy_do_nanosleep(double timesec);

#endif // __JOY_UTILITY_H__
