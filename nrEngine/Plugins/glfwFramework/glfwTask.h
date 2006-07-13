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


#ifndef _NR_GLFW_TASK_H_
#define _NR_GLFW_TASK_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine.h>
#include "glfw.h"

using namespace nrEngine;

#define GLFW_CANNOT_INITIALIZE ( 1 << 0 )
#define GLFW_TASK_ALREADY_RUNNING ( 1 << 1)


/**
 * glfwTask is a task provided by this plugin running in the engine's kernel.
 * The work of a task will be to poll the events from the glfw subsystem
 * and to send events through the engine's communication channel
 * to message listeners.
 *
 * The class also defines some callback functions which will be called
 * from the glfw subsystem and emit some events to nrEngine's message
 * listeners.
 **/
class glfwTask : public ITask{
	public:

		/**
		 * Initialize glfw subsystem and check if such task is
		 * already in the kernel. If it is a case, so throw an error.
		 * The error must be catched by the calling functions, so
		 * we can tell the application which loads the plugin, that
		 * there is already a plugin running such a task.
		 **/
		glfwTask(Engine* root);

		/**
		 * Terminate glfw subsystem and release used memory
		 **/
		~glfwTask();

		/**
		 * update the task
		 **/
		Result taskUpdate();

	private:

		Engine* mEngine;
};



#endif	//_NR...
