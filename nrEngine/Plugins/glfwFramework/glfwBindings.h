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

/***************************************************************************
 *                                                                         *
 * glfwBindings.h -                                                        *
 *        glfwBindings is a plugin for the nrEngine. This header file does *
 *        define all extra features used in this plugin. There are events  *
 *        which are send by the plugin on inputs or on window manipulations*
 *        To properly react on this events you have to know how they are   *
 *        constructed. There are also some classes provided by the plugin. *
 *                                                                         *
 ***************************************************************************/

#ifndef __GLFW_BINDINGS_H_
#define __GLFW_BINDINGS_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/nrEngine.h>

//----------------------------------------------------------------------------------
// Event subsysystem definition of the glfwBindings plugin
//----------------------------------------------------------------------------------

//namespace glfwBindings
//{

	//! Initialize the system
	void glfwBindings_Init(nrEngine::Engine* root);

	/**
	* KeyboardEvent - is commited if there is any changes on the state of keyboard keys.
	*
	* For key mapping we use the mapping already provided with the engine. We do not
	* want to write our own, because then the applications using other plugins for
	* the input may get into troubles.
	*
	**/
	class _NRExport KeyboardEvent : public nrEngine::EventT<KeyboardEvent>{

		public:

			//! Constructor
			KeyboardEvent(nrEngine::keyIndex key) : nrEngine::EventT<KeyboardEvent>(nrEngine::Priority::NORMAL) { mKey = key; }

			//! Destructor non virtual
			~KeyboardEvent() {}

			/**
			* This returns a key index of a key whichs state changes.
			* The message contain information only about one key, so
			* you have to store the states somewhere in between, or use
			* provided plugin methods, to get state information
			* on the keys.
			**/
			nrEngine::keyIndex getKey() { return mKey; }

		private:

			//! Key whichs state is changed
			nrEngine::keyIndex mKey;

	};

	/**
	* KeyboardPressEvent is commited if a key was pressed
	**/
	class KeyboardPressEvent : public KeyboardEvent {
		public:
			KeyboardPressEvent(nrEngine::keyIndex key = nrEngine::KEY_UNKNOWN) : KeyboardEvent(key) {}
	};

	/**
	* KeyboardReleaseEvent would be emited if a key was released
	**/
	class KeyboardReleaseEvent : public KeyboardEvent {
		public:
			KeyboardReleaseEvent(nrEngine::keyIndex key = nrEngine::KEY_UNKNOWN) : KeyboardEvent(key) {}
	};

//}

#endif

