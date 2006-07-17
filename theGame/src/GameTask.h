/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _C_GAME_TASK_H_
#define _C_GAME_TASK_H_

//-------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------
#include "Base.h"
#include "App.h"

/**
 * This is our main application class.
 * Initialize, start, loop and close the application.
 */
class GameTask : public nrEngine::ITask, public nrEngine::EventActor{
	public:

		/**
		 * Create a instance of game task. This will initialize
		 * the task as event actor/listener by the glfw binding engine,
		 * so we recieve events from it
		 **/
		GameTask(App* app);

		/**
		 * Start the task by creating a rendering window and
		 * intializing all useful variables.
		 **/
		nrEngine::Result taskStart();

		/**
		 * Update the game task.
		 **/
		nrEngine::Result taskUpdate();

		/**
		 * Check for incoming events.
		 *	If there is a close window event given us from the glfw binding library
		 * so we will force the application to release the data.
		 **/
		void OnEvent(const nrEngine::EventChannel& channel, SharedPtr<nrEngine::Event> event);

	private:

		//! Application pointer
		App*	mApp;

};

#endif
