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
Result App::init(){

	// check if we are already initialized
	if (mInitialized) return OK;
fprintf(stderr, "HALLO\n");

	// we initialize the nrEngine and setup logging facilities
	Engine::Instantiate();
fprintf(stderr, "HALLO\n");

	Engine::GetSingleton().initializeLog("log/");
	Engine::GetSingleton().initializeEngine();
fprintf(stderr, "HALLO\n");

    // setup logging echoes
	Log::GetSingleton().setEcho(Log::LOG_ENGINE, Log::LOG_CONSOLE);
	Log::GetSingleton().setEcho(Log::LOG_APP, Log::LOG_CONSOLE);
	Log::GetSingleton().setLevel(Log::LL_DEBUG);

fprintf(stderr, "HALLO\n");

	// initialize the glfw binding
	glfw::Binding::Instantiate();
fprintf(stderr, "HALLO\n");

	// create the game task and add it into kernel
	SharedPtr<ITask> game (new GameTask(this));
	Kernel::GetSingleton().AddTask(game, ORDER_NORMAL);
fprintf(stderr, "HALLO\n");

	// ok
	mInitialized = true;
	return OK;
}

//--------------------------------------------------------------------------
Result App::start(){

	// ok
	return OK;
}

//--------------------------------------------------------------------------
Result App::loop(){

	// check for initialization
	if (!mInitialized) return UNKNOWN_ERROR;

	// start the kernel
	Kernel::GetSingleton().Execute();

	// ok
	return OK;
}

//--------------------------------------------------------------------------
void App::stop(){

	if (!mInitialized) return;

	// kill all tasks
	Kernel::GetSingleton().StopExecution();
}

//--------------------------------------------------------------------------
Result App::close(){

	if (!mInitialized) return OK;

	// close the glfw binding
	glfw::Binding::Release();

	// close the engine and deinit all used stuff
	Engine::Release();

	// not initialized anymore
	mInitialized = false;

	// ok
	return OK;
}


