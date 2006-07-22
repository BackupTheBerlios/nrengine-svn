/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "SoundTask.h"

using namespace nrEngine;
using namespace nrBinding;

//--------------------------------------------------------------------------
SoundTask::SoundTask()
{
	// set task name
	setTaskName("SoundTask");

}

//--------------------------------------------------------------------------
nrEngine::Result SoundTask::taskUpdate()
{
	return OK;
}


