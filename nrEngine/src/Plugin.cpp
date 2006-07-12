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
#include "Plugin.h"
#include "Engine.h"

namespace nrEngine {

	//----------------------------------------------------------------------------------
	Plugin::Plugin() : IResource(), mPluginHandle(0)
	{
		setResourceType ("Plugin");

		m_plgInitialize = NULL;
		m_plgError = NULL;
		m_plgRelease = NULL;

	}

	//----------------------------------------------------------------------------------
	Plugin::~Plugin()
	{
		unloadRes();
	}

	//----------------------------------------------------------------------------------
	void* Plugin::getSymbol(const ::std::string& name) const
	{
		return (void*)NR_PLUGIN_GETSYM( mPluginHandle, name.c_str());
	}

	//----------------------------------------------------------------------------------
	Result Plugin::initialize()
	{

		// get version information
		m_plgEngineVersion = (plgEngineVersion)getSymbol("plgEngineVersion");
		m_plgVersionString = (plgVersionString)getSymbol("plgVersionString");

		if (!m_plgEngineVersion || !m_plgVersionString)
		{
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Plugin seems not to be written for the nrEngine");
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "plgVersionString/plgEngineVersion symbols were not found!!!");
			return PLG_SYMBOL_NOT_FOUND;
		}

		// Log this
		NR_Log(Log::LOG_ENGINE, "Plugin found: %s", m_plgVersionString());
		NR_Log(Log::LOG_ENGINE, "Plugin was written for nrEngine v%s", NR_convertVersionToString(m_plgEngineVersion()).c_str());

		// check if plugin is working with the current engine version
		if (m_plgEngineVersion() > nrEngineVersion){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Plugin has got greater version as the engine, so plugin not loaded");
			return PLG_WRONG_VERSION;
		}

		// log something
		NR_Log(Log::LOG_ENGINE | Log::LOG_PLUGIN, "Initialize plugin %s", getResName().c_str());

#define GET_SYMBOL(var, type)\
		{\
			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "Get plugin symbol %s", #type);\
			var = (type)getSymbol(#type);\
			if (!var){\
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Plugin symbol %s was not found", #type);\
				return PLG_SYMBOL_NOT_FOUND;\
			}\
		}

		// Get plugin symbols
		GET_SYMBOL(m_plgInitialize, plgInitialize);
		GET_SYMBOL(m_plgError, plgError);
		GET_SYMBOL(m_plgRelease, plgRelease);
#undef GET_SYMBOL

		// call the function and check for return code
		int result = m_plgInitialize(Engine::GetSingletonPtr());

		// check for error
		if (result != 0){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Plugin returns error %d (%s). See plugin log for more detail information", result, m_plgError(result));
			return PLG_EXTERNAL_ERROR;
		}

		// now get some extra symbols
		/*m_plgGetMethods = (plgGetMethods)getSymbol("plgGetMethods");
		m_plgCall = (plgCall)getSymbol("plgCall");

		if (m_plgGetMethods){

			// get the list of methods provided by this plugin
			m_plgGetMethods(mPlgMethods);

			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "Plugin provides following symbols: ");

			// now go through each of this method and print some log info about it
			for (uint32 i=0; i < mPlgMethods.size(); i++){
				std::string params;				
				for (uint32 j=0; j < mPlgMethods[i].param.size(); j++){
					params += mPlgMethods[i].param[j].name;
					if (j < mPlgMethods[i].param.size() - 1) params += ", ";
				}
				NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "  found  -  %s (%s)", mPlgMethods[i].name.c_str(), params.c_str());
			}		
		}*/
		
		// all right!
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result Plugin::reloadRes()
	{
		return IResource::reloadRes();
	}

	//----------------------------------------------------------------------------------
	Result Plugin::unloadRes()
	{
		// only unload, if we are loaded
		if (mResIsLoaded){

			// call the release function of the plugin
			m_plgRelease();

			// set all symbols to NULL
			m_plgInitialize = NULL;
			m_plgEngineVersion = NULL;
			m_plgVersionString = NULL;
			m_plgError = NULL;
			m_plgRelease = NULL;

			// say that we are not loaded anymore
			mResIsLoaded = false;

			// set the datacount back
			mResDataSize = sizeof(*this);

			// now remove the plugin from the memory
			mResLoader->unloadResource(this);
		}

		// OK
		return OK;
	}


	//----------------------------------------------------------------------------------
	EmptyPlugin::EmptyPlugin() : Plugin()
	{

	}

	//----------------------------------------------------------------------------------
	EmptyPlugin::~EmptyPlugin()
	{

	}

	//----------------------------------------------------------------------------------
	Result EmptyPlugin::unloadRes()
	{
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result EmptyPlugin::reloadRes()
	{
		return OK;
	}

	//----------------------------------------------------------------------------------
	void* EmptyPlugin::getSymbol(const ::std::string& name) const
	{
		return NULL;
	}

};

