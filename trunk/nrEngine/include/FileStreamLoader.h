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


#ifndef _NR_FILESTREAM_RESOURCE_LOADER_H_
#define _NR_FILESTREAM_RESOURCE_LOADER_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ResourceLoader.h"
#include "FileStream.h"


namespace nrEngine{

	//! File stream loader is able to instantiate file stream objects
	/**
	* @see IResourceLoader
	* \ingroup filesys
	**/
	class _NRExport FileStreamLoader : public IResourceLoader{
	public:

		/**
		* Declare supported resource types and supported files.
		**/
		FileStreamLoader();

		/**
		* Destructor
		**/
		~FileStreamLoader();


		/**
		 * Initialize supported resource and file types for the resource file stream
		 * @copydoc IResourceLoader::initialize()
		 **/
		Result initialize();
		
		/**
		* Load a file stream resourse object
		* @copydoc IResourceLoader::loadResource()
		**/
		Result loadResource(IResource* resource);

		/**
		* Instantiate empty file stream object.
		* @param resourceType Unique name of the resource type to be created
		* @return Instance of empty resource
		**/
		IResource* createEmptyResource(const ::std::string& resourceType);


		/**
		* Create an instance of file stream object.
		* @copydoc IResourceLoader::createResourceInstance()
		**/
		IResource* createResourceInstance(const ::std::string& resourceType, NameValuePairs* params = NULL) const;

		/**
		* Unload the file stream.
		* @copydoc IResourceLoader::unloadResource()
		**/
		Result unloadResource(IResource* resource);

	};

};

#endif
