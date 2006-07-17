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

#include "Binding.h"
#include "Task.h"
#include "glfw.h"

namespace nrBinding {
	namespace glfw{

		using namespace nrEngine;

		//----------------------------------------------------------------------
		Binding::Binding()
		{
			// initialize default variables
			mName = "glfwBinding";
			mFullName = "OpenGL Framework Library Binding v0.1 for nrEngine";
			NR_Log(Log::LOG_PLUGIN, "%s: %s",mName.c_str(), mFullName.c_str());

			// check whenever engine is valid
			if (!Kernel::isValid())
			{
				NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: Engine must be initialized before!", mName.c_str());
				NR_EXCEPT(ENGINE_ERROR, "Engine must be initialized before creating a binding", "glfw::Binding::Binding()");
			}

			// initialize glfw task
			try{
				mTask.reset(new Task(Engine::GetSingletonPtr()));
			}catch(int i){
				if (i == GLFW_CANNOT_INITIALIZE){
					NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: Initialization of glfw fails! glfwinit() returns GL_FALSE", mName.c_str());
				}else if (i == GLFW_TASK_ALREADY_RUNNING) {
					NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: You try to start the same plugin twice. glfwBinding_Task is already running", mName.c_str());
				}
			}

			// ok task is created now add it to the kernel
			if (mTask)
			{
				if (Kernel::GetSingleton().AddTask(mTask, ORDER_LAST) == 0){
					NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: glfwBinding_Task could not been added to the kernel", mName.c_str());
				}
			}

		}

		//----------------------------------------------------------------------
		Binding::~Binding()
		{
			// first close the rendering window
			closeWindow();

			// remove the task from the system
			if (Kernel::isValid())
				Kernel::GetSingleton().RemoveTask(mTask->getTaskID());

			// close the task
			mTask.reset();
		}

		//----------------------------------------------------------------------
		const std::string& Binding::getName()
		{
			return mName;
		}

		//----------------------------------------------------------------------
		const std::string& Binding::getFullName()
		{
			return mFullName;
		}

		//----------------------------------------------------------------------
		const std::string& Binding::getChannelName()
		{
			// cast the task to the glfwTask
			return Task::getChannelName();
		}

		//----------------------------------------------------------------------
		bool Binding::createWindow(int32 width, int32 height, bool fullscreen, int32 bpp, int32 depth, int32 stencil)
		{
			// convert given bpp to bits
			int32 r,g,b,a;
			if (bpp == 8){
				r = 3; g = 2; b = 3; a = 0;
			}else if (bpp == 16){
				r = 5; g = 6; b = 5; a = 0;
			}else if (bpp == 24){
				r = 8; g = 8; b = 8; a = 0;
			}else if (bpp == 32){
				r = 8; g = 8; b = 8; a = 8;
			}else{
				NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: Could not match given colorbit depth", mName.c_str());
				return false;
			}

			// create a rendering window as requested
			int32 res = glfwOpenWindow(width, height, r,g,b,a, depth, stencil, fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW);
			if (res == GL_FALSE){
				NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "%s: Could not open a rendering window", mName.c_str());
				return false;
			}

			// now create a callback function, so we get noticed if user close the window
			glfwSetWindowCloseCallback(Binding::closeWindowCallback);

			// now emit an event on the channel, that we got opened a window
			SharedPtr<Event> msg(new OnCreateWindowEvent(width, height, fullscreen, bpp, depth, stencil));
			EventManager::GetSingleton().emit(Binding::getChannelName(), msg);

			// now notice the task, that we have created a window
			boost::dynamic_pointer_cast<Task, ITask>(mTask)->noticeWindowCreated();

			return true;
		}

		//----------------------------------------------------------------------
		void Binding::closeWindow()
		{
			// now emit an event on the channel, that we got to close the window
			SharedPtr<Event> msg(new OnCloseWindowEvent());
			EventManager::GetSingleton().emit(Binding::getChannelName(), msg);

			glfwCloseWindow();
		}

		//----------------------------------------------------------------------
		int32 Binding::closeWindowCallback()
		{
			// now emit an event on the channel, that we got to close the window
			SharedPtr<Event> msg(new OnCloseWindowEvent());
			EventManager::GetSingleton().emit(Binding::getChannelName(), msg);

			return GL_TRUE;
		}

		//----------------------------------------------------------------------
		void Binding::setWindow(const std::string& title, nrEngine::int32 width, nrEngine::int32 height)
		{
			if (width || height) glfwSetWindowSize(width, height);
			glfwSetWindowTitle(title.c_str());
		}

	};
};
