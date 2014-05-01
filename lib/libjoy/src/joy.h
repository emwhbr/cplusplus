/************************************************************************
 *                                                                      *
 * Copyright (C) 2014 Bonden i Nol (hakanbrolin@hotmail.com)            *
 *                                                                      *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 ************************************************************************/

#ifndef __JOY_H__
#define __JOY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/*
 * LIBJOY return codes
 */
#define JOY_SUCCESS               0
#define JOY_FAILURE              -1
#define JOY_ERROR_MUTEX_FAILURE  -2

/*
 * LIBJOY Internal error codes
 */
#define JOY_NO_ERROR                      0
#define JOY_NOT_INITIALIZED               1
#define JOY_ALREADY_INITIALIZED           2
#define JOY_BAD_ARGUMENT                  3
#define JOY_MUTEX_LOCK_FAILED             4
#define JOY_MUTEX_UNLOCK_FAILED           5
#define JOY_FILE_OPERATION_FAILED         6
#define JOY_DEVICE_OPERATION_FAILED       7
#define JOY_WRONG_EVENT_SIZE              8
#define JOY_UNEXPECTED_EXCEPTION          9

/*
 * Error source values
 */
typedef enum {JOY_INTERNAL_ERROR, 
	      JOY_LINUX_ERROR} JOY_ERROR_SOURCE;
/*
 * Basic API support types
 */
#define JOY_EVENT_NONE    0x00
#define JOY_EVENT_BUTTON  0x01
#define JOY_EVENT_AXIS    0x02
#define JOY_EVENT_INIT    0x80

/*
 * API types
 */
typedef char JOY_ERROR_STRING[256];

typedef struct {
  char prod_num[20];
  char rstate[10];
} JOY_LIBJOY_PROD_INFO;

typedef struct {
  JOY_ERROR_SOURCE error_source;
  long             error_code;
} JOY_LIB_STATUS;

typedef enum {JOY_DEV_0,
              JOY_DEV_1} JOY_DEV;  /* Joystick devices */

typedef struct {
  uint8_t type;   /* event type                           */
  int16_t value;  /* axis: -32,767..+32,767, button: 0..1 */
  uint8_t number; /* axis/button number                   */
} JOY_EVENT;

typedef struct {
  float x;     /* cartesian  */
  float y;     /* cartesian  */
  float r;     /* polar      */
  float theta; /* polar      */
} JOY_POS_UNIT_CIRCLE;

#define JOY_X_AXIS  0  /* horizontal axis number */
#define JOY_Y_AXIS  1  /* vertical axis number   */
#define JOY_Z_AXIS  2  /* "twist" axis number    */

/****************************************************************************
*
* Name joy_get_last_error
*
* Description Returns the error information held by LIBJOY, when a LIBJOY 
*             call returns unsuccessful completion. 
*             LIBJOY clears its internal error information after it has been 
*             read by the calling application.
*
* Parameters status  IN/OUT  pointer to a buffer to hold the error information
*
* Error handling Returns JOY_SUCCESS if successful
*                otherwise JOY_FAILURE or JOY_ERROR_MUTEX_FAILURE
*
****************************************************************************/
extern long joy_get_last_error(JOY_LIB_STATUS *status);

/****************************************************************************
*
* Name joy_get_error_string
*
* Description Returns the error string corresponding to the provided
*             internal error code.
*
* Parameters error_code    IN      actual error code
*            error_string  IN/OUT  pointer to a buffer to hold the error string
*
* Error handling Returns always JOY_SUCCESS
*
****************************************************************************/
extern long joy_get_error_string(long error_code, 
				 JOY_ERROR_STRING error_string);

/****************************************************************************
*
* Name joy_initialize
*
* Description Allocates necessary cluster wide resources.
*             This function shall be called once to make LIBJOY operational.
*             This function can be called again after finalization.
*
* Parameters dev        IN  identifies the joystick
*            non_block  IN  mark as non-blocked when reading events
*
* Error handling Returns JOY_SUCCESS if successful
*                otherwise JOY_FAILURE or JOY_ERROR_MUTEX_FAILURE
*
****************************************************************************/
extern long joy_initialize(JOY_DEV dev,
			   bool non_block);

/****************************************************************************
*
* Name joy_finalize
*
* Description Deallocates cluster wide resources created during initialization.
*
* Parameters dev  IN  identifies the joystick
*
* Error handling Returns JOY_SUCCESS if successful
*                otherwise JOY_FAILURE or JOY_ERROR_MUTEX_FAILURE
*
****************************************************************************/
extern long joy_finalize(JOY_DEV dev);

/****************************************************************************
*
* Name joy_get_name
*
* Description Returns the joystick name.
*
* Parameters dev   IN   identifies the joystick
*            name  OUT  pointer to a buffer holding name
*            len   IN   buffer max length
*
* Error handling Returns JOY_SUCCESS if successful
*                otherwise JOY_FAILURE or JOY_ERROR_MUTEX_FAILURE
*
****************************************************************************/
extern long joy_get_name(JOY_DEV dev,
			 char *name,
			 unsigned len);

/****************************************************************************
*
* Name joy_get_axis
*
* Description Returns the number of joystick axis.
*
* Parameters dev   IN   identifies the joystick
*            axis  OUT  pointer to a buffer holding number of axis
*
* Error handling Returns JOY_SUCCESS if successful
*                otherwise JOY_FAILURE or JOY_ERROR_MUTEX_FAILURE
*
****************************************************************************/
extern long joy_get_axis(JOY_DEV dev,
			 int *axis);

/****************************************************************************
*
* Name joy_get_buttons
*
* Description Returns the number of joystick buttons.
*
* Parameters dev      IN   identifies the joystick
*            buttons  OUT  pointer to a buffer holding number of buttons
*
* Error handling Returns JOY_SUCCESS if successful
*                otherwise JOY_FAILURE or JOY_ERROR_MUTEX_FAILURE
*
****************************************************************************/
extern long joy_get_buttons(JOY_DEV dev,
			    int *buttons);

/****************************************************************************
*
* Name joy_get_event
*
* Description Returns an event from joystick.
*
* Parameters dev    IN   identifies the joystick
*            event  OUT  pointer to a buffer holding event
*
* Error handling Returns JOY_SUCCESS if successful
*                otherwise JOY_FAILURE or JOY_ERROR_MUTEX_FAILURE
*
****************************************************************************/
extern long joy_get_event(JOY_DEV dev,
			  JOY_EVENT *event);

/****************************************************************************
*
* Name joy_get_pos_unit_circle
*
* Description Returns joystick position mapped to unit circle.
*
* Parameters x_value  IN   actual x (axis 0) value
*            y_value  IN   actual y (axis 1) value
*            pos      OUT  pointer to a buffer holding joystick position
*
* Error handling Returns always JOY_SUCCESS.
*
****************************************************************************/
extern long joy_get_pos_unit_circle(int16_t x_value,
				    int16_t y_value,
				    JOY_POS_UNIT_CIRCLE *pos);

/****************************************************************************
*
* Name joy_test_get_libjoy_prod_info
*
* Description Returns the product number and the RState of LIBJOY.
*
* Parameters prod_info  IN/OUT  pointer to a buffer to hold the product
*                               number and the RState.
*
* Error handling Returns always JOY_SUCCESS.
*
****************************************************************************/
extern long joy_test_get_libjoy_prod_info(JOY_LIBJOY_PROD_INFO *prod_info);

#ifdef  __cplusplus
}
#endif

#endif /* __JOY_H__ */
