/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _CAPP_H_
#define _CAPP_H_

//-------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------
#include "Base.h"


/**
 * This is our main application class.
 * Initialize, start, loop and close the application.
 */
class App : public nrEngine::ISingleton<App>{
	public:

		/**
		 * Initialize the application tasks and let them run
		 * in the engine's kernel. We also initialize the
		 * engine's bindings and load appropriate plugins
		 **/
		nrEngine::Result init();

		/**
		 * Start the application. Actually do nothing
		 **/
		nrEngine::Result start();

		/**
		 * Go into loop mode, this means Kernel is set into execute
		 * mode, so all task are updated.
		 **/
		nrEngine::Result loop();

		/**
		 * Close a application, by stopping the kernel and
		 * removing all tasks from it. Release also all used memory
		 **/
		nrEngine::Result close();

		/**
		 * Prepare the application to die. This will stop the kernel, so
		 * by the execution loop will hold on. After that you
		 * should call close() to close the application completely
		 **/
		void stop();

	private:

		//! Check if we are already initialized
		bool mInitialized;

};

#endif
