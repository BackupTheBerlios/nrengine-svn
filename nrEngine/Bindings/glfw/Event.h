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

#ifndef __GLFW_BINDINGS_EVENTS_H_
#define __GLFW_BINDINGS_EVENTS_H_


//----------------------------------------------------------------------------------
// Definition of the binding events
//----------------------------------------------------------------------------------
namespace nrBinding{

	namespace glfw{

		/**
		 * OnCreateWindowEvent - this event is emmited as soon as
		 * a new window was created.
		 **/
		class _NRExport OnCreateWindowEvent : public nrEngine::Event{
			public:
				//! Constructor
				OnCreateWindowEvent(nrEngine::int32 width, nrEngine::int32 height,
									bool full, nrEngine::int32 bpp, nrEngine::int32 depth,
									nrEngine::int32 stencil):
					nrEngine::Event(nrEngine::Priority::NORMAL)
					{
						this->width = width;
						this->height = height;
						this->bpp = bpp;
						this->depth = depth;
						this->stencil = stencil;
						this->bFullscreen = full;
					}

				//! width stored in the event
				nrEngine::int32 width;

				//! height of the window
				nrEngine::int32 height;

				//! color buffer bpp
				nrEngine::int32 bpp;

				//! depth buffer bpp
				nrEngine::int32 depth;

				//! stencil buffer bpp
				nrEngine::int32 stencil;

				//! is fullscreen or not
				bool bFullscreen;
		};

		/**
		 * OnCloseWindowEvent - is emmitted if the rendering window
		 * is either closed by the user or by the application. In both cases
		 * if you recieve this message, you have to check, what happens, to
		 * handle appropriately
		 **/
		class _NRExport OnCloseWindowEvent : public nrEngine::Event{
			public:
				OnCloseWindowEvent() : nrEngine::Event(nrEngine::Priority::IMMEDIATE) {}
		};

		/**
		* KeyboardEvent - is commited if there is any changes on the state of keyboard keys.
		*
		* For key mapping we use the mapping already provided with the engine. We do not
		* want to write our own, because then the applications using other plugins for
		* the input may get into troubles.
		*
		**/
		class _NRExport KeyboardEvent : public nrEngine::Event{

			public:

				//! Constructor
				KeyboardEvent(nrEngine::keyIndex key) : nrEngine::Event(nrEngine::Priority::NORMAL) { mKey = key; }

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
		* OnKeyboardPressEvent is commited if a key was pressed
		**/
		class OnKeyboardPressEvent : public KeyboardEvent {
			public:
				OnKeyboardPressEvent(nrEngine::keyIndex key = nrEngine::KEY_UNKNOWN) : KeyboardEvent(key) {}
		};

		/**
		* OnKeyboardReleaseEvent would be emited if a key was released
		**/
		class OnKeyboardReleaseEvent : public KeyboardEvent {
			public:
				OnKeyboardReleaseEvent(nrEngine::keyIndex key = nrEngine::KEY_UNKNOWN) : KeyboardEvent(key) {}
		};

	};
};

#endif

