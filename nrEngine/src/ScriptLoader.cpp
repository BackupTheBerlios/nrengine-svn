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
#include "ScriptLoader.h"
#include "Log.h"
#include "FileStream.h"
#include "Kernel.h"

namespace nrEngine{

	//----------------------------------------------------------------------------------
	ScriptLoader::ScriptLoader()
	{
		initialize();
	}


	//----------------------------------------------------------------------------------
	ScriptLoader::~ScriptLoader()
	{

	}

	//----------------------------------------------------------------------------------
	Result ScriptLoader::initialize(){
		// fill out supported resource types;
		declareSupportedResourceType("nrScript");

		// we do only support dll files in windows version of our engine
		declareSupportedFileType("nrscript");

		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ScriptLoader::unloadResource(IResource* resource)
	{
		// unload
		return IResourceLoader::unloadResource(resource);
	}

	//----------------------------------------------------------------------------------
	Result ScriptLoader::loadResource(IResource* resource)
	{

		// check for errors
		Result res = IResourceLoader::loadResource(resource);
		if (res != OK) return res;

		// load a file so we use its content as a script
		FileStream* fStream = new FileStream();
		Result ret = fStream->open(resource->getResFileName());
		if (ret == OK){

			// get the whole file content
			std::string str = fStream->getAsString();

			// cast the resource to the iscript interface
			IScript* scr = dynamic_cast<IScript*>(resource);

			// load the script from a string
			ret = scr->loadFromString(str);

			// now because each script is also a task, we add this script
			// as a task to the kernel
			SharedPtr<ITask> task (scr);
			Kernel::GetSingleton().AddTask(task);
		}
		delete fStream;

		// return the last error
		return ret;

	}


	//----------------------------------------------------------------------------------
	IResource* ScriptLoader::createResourceInstance(const ::std::string& resourceType, NameValuePairs* params) const
	{

		// check if we do support this kind of resource types
		if (!supportResourceType(resourceType)) return NULL;

		// create an plugin instance
		return new Script();
	}


	//----------------------------------------------------------------------------------
	IResource* ScriptLoader::createEmptyResource(const ::std::string& resourceType)
	{
		// check if we do support this kind of resource types
		if (!supportResourceType(resourceType)) return NULL;

		// create an instance of empty plugin
		return new EmptyScript();
	}


};

