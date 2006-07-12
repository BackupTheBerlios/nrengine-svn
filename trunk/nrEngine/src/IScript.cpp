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


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "IScript.h"
#include "Log.h"

namespace nrEngine{


	//--------------------------------------------------------------------
	Result IScript::unloadRes()
	{
		// only unload, if we are loaded
		if (mResIsLoaded){

			// say that we are not loaded anymore
			mResIsLoaded = false;

			// set the datacount back
			mResDataSize = sizeof(*this);

			// now remove the plugin from the memory
			mResLoader->unloadResource(this);
		}


		return OK;
	}

	//--------------------------------------------------------------------
	Result IScript::reloadRes()
	{
		// if we reload a resource, so we set the task name to the resource name
		setTaskName(std::string("Script_") + getResName());
		
		// do stuff according to the resource 
		return IResource::reloadRes();
	}

	//--------------------------------------------------------------------
	Result IScript::taskUpdate()
	{
		return step();
	}
	
}; // end namespace

