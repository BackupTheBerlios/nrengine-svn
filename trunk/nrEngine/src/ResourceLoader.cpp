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
#include "ResourceLoader.h"
#include "Log.h"


namespace nrEngine{
	
	//----------------------------------------------------------------------------------
	bool IResourceLoader::supportResourceType(const std::string& resourceType) const {
		std::vector<std::string>::const_iterator it;
		for (it = mSupportedResourceTypes.begin(); it != mSupportedResourceTypes.end(); it++){
			if ((*it) == resourceType) return true;
		}
		return false;
	}

	//----------------------------------------------------------------------------------
	bool IResourceLoader::supportFileType(const std::string& fileType) const {
		std::vector<std::string>::const_iterator it;
		for (it = mSupportedFileTypes.begin(); it != mSupportedFileTypes.end(); it++){
			if ((*it) == fileType) return true;
		}
		return false;
	}
	
	//----------------------------------------------------------------------------------
	void IResourceLoader::setResourceEmpty(IResource* res, bool b){
		NR_ASSERT(res != NULL);
		res->mResIsEmpty = b;
	}

	//----------------------------------------------------------------------------------
	Result IResourceLoader::loadResource(IResource* resource)
	{

		// check whenever the given pointer is valid
		if (resource == NULL)
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "The given resource pointer is NULL.");
			return BAD_PARAMETERS | RES_PTR_IS_NULL;
		}
		
		// check whenever we do support this kind of resources
		if (!supportResourceType(resource->getResType()))
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Given resource is not supported by this loader");
			return RES_TYPE_NOT_SUPPORTED;
		}

		return OK;
	}
	
	//----------------------------------------------------------------------------------
	Result IResourceLoader::unloadResource(IResource* resource)
	{
		// check whenever the given pointer is valid
		if (resource == NULL)
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "The given resource pointer is NULL.");
			return BAD_PARAMETERS | RES_PTR_IS_NULL;
		}
		
		// check whenever we do support this kind of resources
		if (!supportResourceType(resource->getResType()))
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Given resource is not supported by this loader");
			return RES_TYPE_NOT_SUPPORTED;
		}
		
		// declare a static variable to prevent deadlocks
		static bool mutex = false;
		
		// check if the resource is already unloaded
		if (resource->isResLoaded() && mutex == false) {
			mutex = true;
			resource->unloadRes();
		}else{
			mutex = false;
		}

		return OK;
	}
	
};

