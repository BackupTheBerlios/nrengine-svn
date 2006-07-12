/***************************************************************************
 *                                                                         *
 *   (c) Art Tevs, MPI Informatik Saarbruecken                             *
 *       mailto: <tevs@mpi-sb.mpg.de>                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef _NR_ENGINE_EVENT__H_
#define _NR_ENGINE_EVENT__H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "../Prerequisities.h"
#include "../Event.h"

/*!
 * \defgroup sysevent System events
 *
 * This group represents all system events used in the engine.
 * The events are send trough the default engine's communication
 * channel. Very important events will be send on every channel, so
 * user application get noticed about them.
 *
 * The event interface is usefull for appropriate work of the engine.
 * All events are handled in the background fully transparent to the user,
 * so you do not have to care about them.
 *
 * Some of the events could only be created by the system, so normal
 * application is not allowed to send messages of this type. This is done
 * by declaring the event constructor as protected/private, so only friend
 * classes can create such events. You as normal user either don't need them or
 * uncontrolled sending of them could cause system damage. However you are able
 * to recieve the messages sended by the engine, if you need them this is another
 * question ;-)
 * 
 **/


#endif
