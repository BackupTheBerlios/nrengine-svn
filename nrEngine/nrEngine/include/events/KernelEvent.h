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


#ifndef _NR_ENGINE_KERNEL_EVENT__H_
#define _NR_ENGINE_KERNEL_EVENT__H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "EngineEvent.h"
#include "../ITask.h"
#include "../Kernel.h"

namespace nrEngine{

	//! On of the system base events classes (used by kernel to send information)
	/**
	 * KernelEvent is a base class for all possible event classes
	 * according to the tasking and kernel work. Kernel could
	 * send this events around if he thinks the application should
	 * be informed about the things going on.
	 *
	 * \ingroup sysevent
	 **/
	class _NRExport KernelEvent : public Event {
		public:

			/**
			 * Get id of a task about which this message is sent.
			 **/
			const taskID& getTaskID() const { return mTaskId; }

			/**
			 * Get the name of the task of the message
			 **/
			const std::string& getName() const { return mTaskName; }

		protected:

			//! Only kernel is allowed to change the values here
			friend class Kernel;

			/**
			 * The constructor is protected, so only friend namely kernel
			 * is allowed to create events of this type. So think
			 * as if this event were a system event which you are not able
			 * to send by yourself. Only kernel subsystem could send them around.
			 **/
			KernelEvent(const std::string& taskName, taskID id, Priority prior = Priority::IMMEDIATE);

			//! Store the id of a task about which we want to send information
			taskID	mTaskId;

			//! Store the task name
			std::string mTaskName;

	};

}; // end namespace

#endif
