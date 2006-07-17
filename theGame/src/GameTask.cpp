/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "GameTask.h"
#include "App.h"

using namespace nrEngine;
using namespace nrBinding;

//--------------------------------------------------------------------------
GameTask::GameTask(App* app) : EventActor("GameTask"), mApp (app)
{
	// set task name
	setTaskName("GameTask");

	// register the task as a listener on glfw channel
	connect(glfw::Binding::getChannelName());
}

//--------------------------------------------------------------------------
Result GameTask::taskStart()
{
	// create a window
	glfw::Binding::GetSingleton().createWindow(640, 480, false);
	glfw::Binding::GetSingleton().setWindow("theGame - v0.1");

	// ok
	return OK;
}

//--------------------------------------------------------------------------
Result GameTask::taskUpdate()
{
	// ok
	return OK;
}

//--------------------------------------------------------------------------
void GameTask::OnEvent(const nrEngine::EventChannel& channel, SharedPtr<nrEngine::Event> event)
{
	// Is it a keypress event, then check for the key and close
	// application if it was escape, key
	if (event->same_as<glfw::OnKeyboardPressEvent>())
	{
		SharedPtr<glfw::OnKeyboardPressEvent> ev = event_shared_cast<glfw::OnKeyboardPressEvent>(event);
		if (ev->getKey() == KEY_ESCAPE){
			mApp->stop();
		}

	// if a window close event is apears, so close it
	}else if (event->same_as<glfw::OnCloseWindowEvent>()){
		mApp->stop();
	}
}


