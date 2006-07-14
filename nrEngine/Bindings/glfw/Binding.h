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

#ifndef __GLFW_BINDINGS_PLUGIN_H_
#define __GLFW_BINDINGS_PLUGIN_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/nrEngine.h>
#include "Event.h"

namespace nrBinding {

	namespace glfw {

		//! Main class of the glfw binding
		/**
		* Binding is a main class of the glfw loadtime plugin (or binding).
		* It is declared as a singleton, so it can be created only once.
		* This will initialize the subsysems like glfw framework and
		* event management. So you have to create a instance of this class
		* to provide the user the functionality of this binding.
		**/
		class _NRExport Binding : public nrEngine::Binding, public nrEngine::ISingleton<Binding>{

			public:

				/**
				* Default constructor used, to create instance and
				* to initialize all underlying systems.
				*
				* NOTE: You have to instantiate a class after initialization
				* 		of the engine, otherwise exception will be thrown
				**/
				Binding();

				/**
				* Release used memory. Close glfw subsystem and close
				* the task running in the kernel
				**/
				~Binding();

				/**
				 * @see nrEngine::Binding::getName()
				 **/
				const std::string& getName();

				/**
				 * @see nrEngine::Binding::getFullName()
				 **/
				const std::string& getFullName();

				/**
				 * Open a window where all OpenGL operations access on.
				 *
				 * @param width,height Size of the window
				 * @param fullscreen If true, create fullscreen
				 * @param bpp Colorbuffer bit depth (default 32)
				 * @param depth Depthbuffer bit depth (default 24)
				 * @param stencil Stencilbuffer bit depth (default 8)
				 * @return true if it succeeds, false otherwise. See log files
				 **/
				bool createWindow(nrEngine::int32 width, nrEngine::int32 height, bool fullscreen, nrEngine::int32 bpp = 32, nrEngine::int32 depth = 24, nrEngine::int32 stencil = 8);

				/**
				 * Close the window and kill all used OpenGL contexts
				 **/
				void closeWindow();

				/**
				 * Get the name of the message channel where events are
				 * published
				 **/
				static const std::string& getChannelName();

			private:

				//! Task object of the bindings
				SharedPtr<nrEngine::ITask>	mTask;

				//! Name of the binding
				std::string mName;

				//! Full name
				std::string mFullName;

				//! Callback function to close the window
				static nrEngine::int32 closeWindowCallback();

		};
	};
};

#endif

