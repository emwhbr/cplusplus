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

#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/select.h>
#include <sys/time.h> 
#include <linux/joystick.h>

#include "joy_io.h"
#include "joy_exception.h"
#include "joy_utility.h"
#include "joy_hw.h"

// Implementation notes:
// 1. General
//    https://www.kernel.org/doc/Documentation/input/joystick.txt
//    https://www.kernel.org/doc/Documentation/input/joystick-api.txt
//
// 2. Examples
//    http://www.keithlantz.net/2011/10/a-linux-c-joystick-object/
//    http://scaryreasoner.wordpress.com/2008/02/22/programming-joysticks-with-linux/
//
// 3. Axis
//            -32767 | Axis=1 (Y)
//                   |
//                   |
//                   |
//                   |
//                   |
//                   |
// ------------------------------------- Axis=0 (X)
// -32767            |             32767
//                   |
//                   |
//                   |
//                   |
//                   |
//                   | 32767
//                          

/////////////////////////////////////////////////////////////////////////////
//               Definitions of macros
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Public member functions
/////////////////////////////////////////////////////////////////////////////

joy_io::joy_io(string joy_dev)
{
  m_joy_dev = joy_dev;

  init_members();
}

////////////////////////////////////////////////////////////////

joy_io::~joy_io(void)
{
}

////////////////////////////////////////////////////////////////

void joy_io::init(bool non_block)
{
  int rc;

  // Open joystick device
  rc = open(m_joy_dev.c_str(), O_RDONLY);
  if (rc == -1) {
    THROW_EXP(JOY_LINUX_ERROR, JOY_FILE_OPERATION_FAILED,
	      "open failed, device (%s)",
	      m_joy_dev.c_str());
  }
  m_joy_fd = rc;

  // Apply advisory lock
  // Only one process may have access to joystick
  if ( flock(m_joy_fd, LOCK_EX | LOCK_NB) == -1 ) {
    THROW_EXP(JOY_LINUX_ERROR, JOY_FILE_OPERATION_FAILED,
	      "flock failed - exclusive, device (%s)",
	      m_joy_dev.c_str());
  }

  // Identify joystick
  if ( ioctl(m_joy_fd, JSIOCGNAME(sizeof(m_name)), m_name) < 0 ) {
    close(m_joy_fd);
    THROW_EXP(JOY_LINUX_ERROR, JOY_DEVICE_OPERATION_FAILED,
	      "Failed to get name, device(%s)",
	      m_joy_dev.c_str());
  }
  if ( ioctl(m_joy_fd, JSIOCGAXES, &m_axis) < 0 ) {
    close(m_joy_fd);
    THROW_EXP(JOY_LINUX_ERROR, JOY_DEVICE_OPERATION_FAILED,
	      "Failed to get axis, device(%s)",
	      m_joy_dev.c_str());
  }
  if ( ioctl(m_joy_fd, JSIOCGBUTTONS, &m_buttons) < 0 ) {
    close(m_joy_fd);
    THROW_EXP(JOY_LINUX_ERROR, JOY_DEVICE_OPERATION_FAILED,
	      "Failed to get buttons, device(%s)",
	      m_joy_dev.c_str());
  }

  m_non_block = non_block;
}

////////////////////////////////////////////////////////////////

void joy_io::finalize(void)
{
  // Remove advisory lock
  if ( flock(m_joy_fd, LOCK_UN) == -1 ) {
    THROW_EXP(JOY_LINUX_ERROR, JOY_FILE_OPERATION_FAILED,
	      "flock failed - remove, device (%s)",
	      m_joy_dev.c_str());
  }

  // Close joystick device
  if ( close(m_joy_fd) == -1 ) {
    THROW_EXP(JOY_LINUX_ERROR, JOY_FILE_OPERATION_FAILED,
	      "close failed for device file %s",
	      m_joy_dev.c_str());
  }
  
  // Reset members
  init_members();
}

////////////////////////////////////////////////////////////////

long joy_io::get_name(char *name,
		      unsigned len)
{
  if ( strlen(m_name) >= len ) {
    strncpy(name, m_name, len - 1);
    name[len-1] = '\0';
  }
  else {
    strncpy(name, m_name, len);
  }

  return JOY_SUCCESS;
}

////////////////////////////////////////////////////////////////

long joy_io::get_axis(int *value)
{
  *value = (unsigned) m_axis;
  return JOY_SUCCESS;
}

////////////////////////////////////////////////////////////////

long joy_io::get_buttons(int *value)
{
  *value = (unsigned) m_buttons;
  return JOY_SUCCESS;
}

////////////////////////////////////////////////////////////////

long joy_io::get_event(JOY_EVENT *event)
{
  int rc;
  struct js_event jse;
  struct timeval tv = {0, 0};
  fd_set fds;  

  FD_ZERO(&fds);
  FD_SET(m_joy_fd, &fds);

  // Wait for input from joystick
  if (m_non_block) {
    rc = select(m_joy_fd+1, &fds, NULL, NULL, &tv);

    // Check for timeout
    if (rc == 0) {
      event->type   = JOY_EVENT_NONE;
      event->value  = 0;
      event->number = 0;
      return JOY_SUCCESS;
    }
  }
  else {
    rc = select(m_joy_fd+1, &fds, NULL, NULL, NULL);
  }  

  if (rc == -1) {
    THROW_EXP(JOY_LINUX_ERROR, JOY_FILE_OPERATION_FAILED,
	      "select failed for device file %s",
	      m_joy_dev.c_str());
  }

  // Data ready, continue to read
  rc = read(m_joy_fd, &jse, sizeof(jse));
  if (rc == -1) {
    THROW_EXP(JOY_LINUX_ERROR, JOY_FILE_OPERATION_FAILED,
	      "read failed for device file %s",
	      m_joy_dev.c_str());
  }
  if (rc != sizeof(jse)) {
    THROW_EXP(JOY_INTERNAL_ERROR, JOY_WRONG_EVENT_SIZE,
	      "read got size(%d), expected(%d) for device file %s",
	      rc, sizeof(jse), m_joy_dev.c_str());
  }

  // Copy data to caller
  event->type   = jse.type;
  event->value  = jse.value;
  event->number = jse.number;

  return JOY_SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
//               Protected member functions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//               Private member functions
/////////////////////////////////////////////////////////////////////////////

void joy_io::init_members(void)
{
  m_joy_fd = 0;
  bzero(m_name, sizeof(m_name));
  m_non_block = false;
  m_axis = 0;
  m_buttons = 0;
}
