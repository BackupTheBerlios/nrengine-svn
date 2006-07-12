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
#include "FileStreamLoader.h"
#include "Log.h"

namespace nrEngine{

	//----------------------------------------------------------------------------------
	FileStreamLoader::FileStreamLoader()
	{
		initialize();
	}


	//----------------------------------------------------------------------------------
	FileStreamLoader::~FileStreamLoader()
	{

	}

	//----------------------------------------------------------------------------------
	Result FileStreamLoader::initialize(){
		// fill out supported resource types;
		declareSupportedResourceType("File");

		return OK;
	}
	
	//----------------------------------------------------------------------------------
	Result FileStreamLoader::loadResource(IResource* resource)
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

		// create a pointer to the stream object and open the file
		FileStream* fileStream = dynamic_cast<FileStream*>(resource);
		return fileStream->open(resource->getResFileName());

	}


	//----------------------------------------------------------------------------------
	Result FileStreamLoader::unloadResource(IResource* resource)
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
		}

		// close the file stream and remove the pointer
		FileStream* fileStream = dynamic_cast<FileStream*>(resource);

		fileStream->mStream->close();
		fileStream->mStream.reset();

		// OK
		return OK;
	}


	//----------------------------------------------------------------------------------
	IResource* FileStreamLoader::createResourceInstance(const ::std::string& resourceType, NameValuePairs* params) const
	{

		// check if we do support this kind of resource types
		if (!supportResourceType(resourceType)) return NULL;

		// create an plugin instance
		return new FileStream();
	}


	//----------------------------------------------------------------------------------
	IResource* FileStreamLoader::createEmptyResource(const ::std::string& resourceType)
	{
		// check if we do support this kind of resource types
		if (!supportResourceType(resourceType)) return NULL;

		// create an instance of empty plugin
		return new EmptyFileStream();
	}

};

