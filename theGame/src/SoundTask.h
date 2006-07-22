/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _C_SOUND_TASK_H_
#define _C_SOUND_TASK_H_

//-------------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------------
#include "Base.h"

/**
 * SoundTask does provide some functions to the game
 * for playing the sounds. It can run parallel in a thread.
 */
class SoundTask : public nrEngine::ITask{
	public:

		/**
		 * Initialize the sound subsystem
		 **/
		SoundTask();

		/**
		 * Update the sound task.
		 **/
		nrEngine::Result taskUpdate();

};

#endif
