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


#include "Task.h"
#include "glfw.h"
#include "Event.h"

#include <boost/function.hpp>
#define TASK_NAME "glfwBinding_Task"

namespace nrBinding {

	namespace glfw{

		std::string Task::ChannelName(TASK_NAME);

		using namespace nrEngine;

		//------------------------------------------------------------
		Task::Task(nrEngine::Engine* root) : mEngine(root)
		{
			setTaskName(TASK_NAME);

			NR_Log(Log::LOG_PLUGIN, "glfwBindings: Initialize the glfw subsystem (OpenGL Framework)");

			// check for engine
			if (mEngine == NULL) throw(-1);

			// check if such a task already running
			if (mEngine->getKernel()->getTaskByName(TASK_NAME)) throw GLFW_TASK_ALREADY_RUNNING;

			// ok let's initialize the glfw subsystem
			if (glfwInit() == GL_FALSE) throw GLFW_CANNOT_INITIALIZE;

		}

		//------------------------------------------------------------
		Task::~Task()
		{
			NR_Log(Log::LOG_PLUGIN, "glfwBindings: Terminate the glfw subsystem");
			glfwTerminate();
		}

		//------------------------------------------------------------
		Result Task::taskInit()
		{
			// create a communication channel
			EventManager::GetSingleton().createChannel(Task::ChannelName);

			// ok
			return OK;
		}

		//------------------------------------------------------------
		void Task::noticeWindowCreated()
		{
			// propaget the key callback function
			glfwSetKeyCallback(keyCallback);
			glfwSetCharCallback(keyCharCallback);
		}

		//------------------------------------------------------------
		const std::string& Task::getChannelName(){
			return Task::ChannelName;
		}


		//------------------------------------------------------------
		Result Task::taskUpdate()
		{
			// swap the buffers and auto poll events
			glfwSwapBuffers();

			return OK;
		}

		//------------------------------------------------------------
		void Task::keyCallback(int key, int action)
		{
			// first convert the key into suitable nrEngine's key index
			keyIndex nrkey = convert(key);
			SharedPtr<Event> msg;

			// the key was pressed
			if (action == GLFW_PRESS){

				// send a message that the key was pressed
				msg.reset(new OnKeyboardPressEvent(nrkey));

			// key is released
			}else if (action == GLFW_RELEASE){

				// send a message that key was released
				msg.reset(new OnKeyboardReleaseEvent(nrkey));

			}

			// send to the input chanel
			EventManager::GetSingleton().emit(Task::getChannelName(), msg);
		}

		//------------------------------------------------------------
		void Task::keyCharCallback(int character, int action)
		{

		}


		//------------------------------------------------------------
		keyIndex Task::convert(int key)
		{
		#define MAP(glfw, nrkey) if (key == glfw) return nrkey;

			MAP(GLFW_KEY_SPACE, KEY_SPACE)
			MAP(GLFW_KEY_ESC, KEY_ESCAPE)

			MAP(GLFW_KEY_F1, KEY_F1)
			MAP(GLFW_KEY_F2, KEY_F2)
			MAP(GLFW_KEY_F3, KEY_F3)
			MAP(GLFW_KEY_F4, KEY_F4)
			MAP(GLFW_KEY_F5, KEY_F5)
			MAP(GLFW_KEY_F6, KEY_F6)
			MAP(GLFW_KEY_F7, KEY_F7)
			MAP(GLFW_KEY_F8, KEY_F8)
			MAP(GLFW_KEY_F9, KEY_F9)
			MAP(GLFW_KEY_F10, KEY_F10)
			MAP(GLFW_KEY_F11, KEY_F11)
			MAP(GLFW_KEY_F12, KEY_F12)
			MAP(GLFW_KEY_F13, KEY_F13)
			MAP(GLFW_KEY_F14, KEY_F14)
			MAP(GLFW_KEY_F15, KEY_F15)

			MAP(GLFW_KEY_UP, KEY_UP)
			MAP(GLFW_KEY_DOWN, KEY_DOWN)
			MAP(GLFW_KEY_LEFT, KEY_LEFT)
			MAP(GLFW_KEY_RIGHT, KEY_RIGHT)

			MAP(GLFW_KEY_LSHIFT, KEY_LSHIFT)
			MAP(GLFW_KEY_RSHIFT, KEY_RSHIFT)
			MAP(GLFW_KEY_LALT, KEY_LALT)
			MAP(GLFW_KEY_RALT, KEY_RALT)
			MAP(GLFW_KEY_LCTRL, KEY_LCTRL)
			MAP(GLFW_KEY_LCTRL, KEY_LCTRL)

			MAP(GLFW_KEY_TAB, KEY_TAB)
			MAP(GLFW_KEY_ENTER, KEY_RETURN)
			MAP(GLFW_KEY_BACKSPACE, KEY_BACKSPACE)
			MAP(GLFW_KEY_INSERT, KEY_INSERT)
			MAP(GLFW_KEY_DEL, KEY_DELETE)
			MAP(GLFW_KEY_PAGEUP, KEY_PAGEUP)
			MAP(GLFW_KEY_PAGEDOWN, KEY_PAGEDOWN)
			MAP(GLFW_KEY_HOME, KEY_HOME)
			MAP(GLFW_KEY_END, KEY_END)

			MAP(GLFW_KEY_KP_0, KEY_KP0)
			MAP(GLFW_KEY_KP_1, KEY_KP1)
			MAP(GLFW_KEY_KP_2, KEY_KP2)
			MAP(GLFW_KEY_KP_3, KEY_KP3)
			MAP(GLFW_KEY_KP_4, KEY_KP4)
			MAP(GLFW_KEY_KP_5, KEY_KP5)
			MAP(GLFW_KEY_KP_6, KEY_KP6)
			MAP(GLFW_KEY_KP_7, KEY_KP7)
			MAP(GLFW_KEY_KP_8, KEY_KP8)
			MAP(GLFW_KEY_KP_9, KEY_KP9)

			MAP(GLFW_KEY_KP_DIVIDE, KEY_KP_DIVIDE)
			MAP(GLFW_KEY_KP_MULTIPLY, KEY_KP_MULTIPLY)
			MAP(GLFW_KEY_KP_SUBTRACT, KEY_KP_MINUS)
			MAP(GLFW_KEY_KP_ADD, KEY_KP_PLUS)
			MAP(GLFW_KEY_KP_DECIMAL, KEY_KP_PERIOD)
			MAP(GLFW_KEY_KP_EQUAL, KEY_KP_EQUALS)
			MAP(GLFW_KEY_KP_ENTER, KEY_KP_ENTER)

		#undef MAP
			return KEY_UNKNOWN;
		}
	};
};

