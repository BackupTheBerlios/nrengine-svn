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
#include "PluginLoader.h"
#include "Log.h"

namespace nrEngine{
	
	//----------------------------------------------------------------------------------
	PluginLoader::PluginLoader()
	{
		initialize();
	}

	
	//----------------------------------------------------------------------------------
	PluginLoader::~PluginLoader()
	{
	
	}
	
	//----------------------------------------------------------------------------------
	Result PluginLoader::initialize(){
		// fill out supported resource types;
		declareSupportedResourceType("Plugin");
		declareSupportedResourceType("nrPlugin");
		
		// we do only support dll files in windows version of our engine
#if NR_PLATFORM == NR_PLATFORM_WIN32
		declareSupportedFileType("dll");
#else
		declareSupportedFileType("so");
#endif
		return OK;
	}
		
	//----------------------------------------------------------------------------------
	Result PluginLoader::loadResource(IResource* resource)
	{

		// check for errors
		Result res = IResourceLoader::loadResource(resource);
		if (res != OK) return res;

		// check whenever we have a valid file name of the plugin
		std::string name = resource->getResFileName();
		
		if (name.length() <= 3){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "File name of the plugin is not valid %s. It must contain at least 3 characters.", name.c_str());
			return RES_BAD_FILETYPE;
		}
		
#if NR_PLATFORM == NR_PLATFORM_LINUX
		if (name.substr(name.length() - 3, 3) != ".so")
		{
			// dlopen() does not add .so to the filename, like windows does for .dll
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "\".so\" added to the plugin file name %s", name.c_str());
			name += ".so";
		}
#elif NR_PLATFORM == NR_PLATFORM_WIN32
		if (name.substr(name.length() - 4, 4) == ".dll")
		{
			// windows does automaticly add .dll to the file name, so we have to cut the name
			name = name.substr(0, name.length() - 4);
		}		
#endif
		
		// cast the resource pointer to appropriate one
		if (resource->getResType() == "Plugin" || resource->getResType() == "nrPlugin")
		{
			Plugin* plugin = dynamic_cast<Plugin*>(resource);
			 
			// now load the library
			plugin->mPluginHandle = (PluginHandle)NR_PLUGIN_LOAD(plugin->getResFileName().c_str());

			// check whenever the library could be loaded
			if (plugin->mPluginHandle == NULL)
			{
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, 
					"Plugin %s could not be loaded! System Msg: %s", 
					plugin->getResName().c_str(), getLastPluginError().c_str());
				return PLG_COULD_NOT_LOAD;
			}
			
			// force the plugin resource object to initialize itself after loading
			return plugin->initialize();
		}

		// we do not have valid resource type
		return RES_TYPE_NOT_SUPPORTED;
				
	}
	
	
	//----------------------------------------------------------------------------------
	Result PluginLoader::unloadResource(IResource* resource)
	{
		// check for errors
		Result res = IResourceLoader::unloadResource(resource);
		if (res != OK) return res;
		
		// now unload the library from the memory
		if (resource->getResType() == "Plugin" || resource->getResType() == "nrPlugin")
		{
			Plugin* plugin = dynamic_cast<Plugin*>(resource);

			if (NR_PLUGIN_UNLOAD(plugin->mPluginHandle))
			{
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, 
					"Could not unload plugin %s. System Msg: %s", 
					plugin->getResName().c_str(), getLastPluginError().c_str());
				return PLG_UNLOAD_ERROR;
			}
			
			// set the handle to 0
			plugin->mPluginHandle = NULL;
		}
		
		// OK
		return OK;		
	}
	
	
	//----------------------------------------------------------------------------------
	IResource* PluginLoader::createResourceInstance(const ::std::string& resourceType, NameValuePairs* params) const
	{
	
		// check if we do support this kind of resource types
		if (!supportResourceType(resourceType)) return NULL;
	
		// create an plugin instance
		return new Plugin();
	}
	
	
	//----------------------------------------------------------------------------------
	IResource* PluginLoader::createEmptyResource(const ::std::string& resourceType)
	{
		// check if we do support this kind of resource types
		if (!supportResourceType(resourceType)) return NULL;

		// create an instance of empty plugin
		return new EmptyPlugin();	
	}
	
	
	//-----------------------------------------------------------------------
	std::string PluginLoader::getLastPluginError() 
	{
#if NR_PLATFORM == NR_PLATFORM_WIN32
		LPVOID lpMsgBuf; 
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, 
			GetLastError(), 
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
			(LPTSTR) &lpMsgBuf, 
			0, 
			NULL 
			); 
		std::string ret = (char*)lpMsgBuf;
		
		// Free the buffer.
		LocalFree( lpMsgBuf );
		return ret;
#elif NR_PLATFORM == NR_PLATFORM_LINUX
		return std::string(dlerror());
#elif NR_PLATFORM == NR_PLATFORM_APPLE
		return std::string(mac_errorBundle());
#else
		return std::string("");
#endif
	}

};

