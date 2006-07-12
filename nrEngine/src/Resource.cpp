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
#include "Resource.h"

namespace nrEngine {

	//----------------------------------------------------------------------------------
	IResource::IResource()
	{
		mResIsLoaded = false;
		mResHandle = 0;
		mResIsEmpty = false;
		mParentManager = NULL;

		mResDataSize = sizeof(*this);
	}

	//----------------------------------------------------------------------------------
	IResource::~IResource()
	{

	}

	//----------------------------------------------------------------------------------
	Result IResource::addToResourceManager(ResourceManager& manager, const std::string& name, const std::string group)
	{
		return manager.add(this, name, group);
	}

	//----------------------------------------------------------------------------------
	Result IResource::reloadRes()
	{
		// first we unload the resource
		Result ret = unloadRes();
		if (ret != OK) return ret;

		// now force the loader to load the plugin again
		ret = mResLoader->loadResource(this);
		if (ret != OK) return ret;

		// now setup internal variables
		mResIsLoaded = true;

		// set the datacount back
		mResDataSize = sizeof(*this);

		return OK;
	}

};

