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

 
#include "Engine.h"
#include "Kernel.h"
#include "Clock.h"
#include "Profiler.h"
#include "ResourceManager.h"
#include "PluginLoader.h"
#include "FileStreamLoader.h"
#include "EventManager.h"
#include "ScriptEngine.h"

namespace nrEngine{

	//------------------------------------------------------------------------
	Engine::Engine()
	{
		// first of all create the log system, so we get valid pointer
		_logger.reset(new Log());
		if (_logger == NULL)
		{
			NR_EXCEPT(OUT_OF_MEMORY, "Log system could not be created. Probably memory is full", "Engine::Engine()");
		}

		// now create a kernel subsystem
		_kernel.reset(new Kernel());
		if (_kernel == NULL)
		{
			NR_EXCEPT(OUT_OF_MEMORY, "Kernel system could not be created. Probably memory is full", "Engine::Engine()");
		}

		// initialize the clock
		_clock.reset(new Clock());
		if (_clock == NULL)
		{
			NR_EXCEPT(OUT_OF_MEMORY, "Clock could not be created. Probably memory is full", "Engine::Engine()");
		}
			
	}

	//------------------------------------------------------------------------
	Engine::~Engine()
	{
		// delete the scripting engine
		_script.reset();

		// delete the event system
		_event.reset();

		// remove the manager
		_resmgr.reset();
				
		// delete the clock
		_clock.reset();
		
		// delete the kernel
		_kernel.reset();

		// remove profiler
		_profiler.reset();
						
		// delete the log system
		_logger.reset();

	}

	//------------------------------------------------------------------------
	bool Engine::initializeLog(const std::string& logPath)
	{
		return _logger->initialize(logPath) == OK;
	}

	//------------------------------------------------------------------------
	void Engine::stopEngine()
	{
		// give log information
		_logger->log(Log::LOG_ENGINE, "nrEngine stopped");
		
		// stop kernel tasks
		Kernel::GetSingleton().StopExecution();

	}
	//------------------------------------------------------------------------
	bool Engine::initializeEngine()
	{
		// give some info about the underlying engine
		NR_Log(Log::LOG_ENGINE | Log::LOG_CONSOLE | Log::LOG_KERNEL, "nrEngine v%s - %s", NR_convertVersionToString(nrEngineVersion).c_str(), NR_VERSION_NAME);
			
		// detect the cpu and write some log info
		//NR_Log(Log::LOG_ENGINE, "Retrieve CPU Information");
		//_cpu->detect();
		//NR_Log(Log::LOG_ENGINE, "%s", _cpu->getCPUDescription().c_str());
		
		// initialize the clock
		SharedPtr<TimeSource> timeSource(new TimeSource());

		// initialize profiler singleton
		_profiler.reset(new Profiler(timeSource));
		if (_profiler == NULL)
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Profiler singleton could not be created. Probably memory is full");

		// now add the clock into kernel
		_clock->setTimeSource(timeSource);
		_clock->setTaskType(TASK_SYSTEM);
		_clock->AddToKernel((*_kernel), ORDER_SYS_FIRST);
		
		// initialize the scripting engine
		_script.reset(new ScriptEngine());
		if (_script == NULL)
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Scripting Engine could not been created. Check if the memory is not full");
			
		// initialize resource manager singleton
		_resmgr.reset(new ResourceManager());
		if (_resmgr == NULL)
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Resource manager singleton could not be created. Probably memory is full");		

		// initialize resource manager singleton
		_event.reset(new EventManager());
		if (_event == NULL)
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Event manager singleton could not be created. Probably memory is full");

		_event->setTaskType(TASK_SYSTEM);
		_event->AddToKernel((*_kernel), ORDER_SYS_SECOND);
		
		// Add the file reading functionality
		SharedPtr<FileStreamLoader> fileLoader (new FileStreamLoader());
		_resmgr->registerLoader("FileStreamLoader", fileLoader);
				
		// create an instance of plugin loader and add it to the resource manager
		ResourceLoader loader ( new PluginLoader() );
		_resmgr->registerLoader("PluginLoader", loader);

		// create default event communication channel to allow
		// comunication between engine's components
		_event->createChannel(NR_DEFAULT_EVENT_CHANNEL);
		
		return true;
	}
	
	//------------------------------------------------------------------------
	void Engine::runEngine()
	{
	
		// log info
		_logger->log(Log::LOG_ENGINE, "nrEngine started");

		// start the kernel
		_kernel->Execute();
		
	}

	//------------------------------------------------------------------------
	void Engine::updateEngine()
	{
		// update the kernel
		_kernel->OneTick();
	}

			
	//------------------------------------------------------------------------
	bool Engine::loadPlugin(const std::string& path, const std::string& file, const std::string& name)
	{
		// check if the engine is initialized
		if (_resmgr == NULL){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Engine was not initialized properly");
			return UNKNOWN_ERROR;
		}
		
		// try to load the resource and check for the error code
		IResourcePtr plg = _resmgr->loadResource(name, "Plugins", "Plugin", path + file);

		if (plg == NULL)
			return false;

		return true;
	}
	
	//------------------------------------------------------------------------
	//Plugin getPlugin(const std::string& name){
		
	//}

	
}; // end namespace

#if 0
/**
 * This function will convert the given nrResult number to the
 * string describing it. e.g: NR_OK => "NR_OK"
 *
 * @param res - error code to be converted in string
 * \ingroup engine
 **/
string	nrErrorGetShort(nrResult res){

	char str[256];
	sprintf(str, "%x", res);

#define _ERROR(p) case p: sprintf(str, #p);break;	
	
	switch (res){
	
_ERROR(NR_OK)
_ERROR(NR_UNKNOWN_ERROR)
_ERROR(NR_BAD_PARAMETERS)
_ERROR(NR_OUT_OF_MEMORY)

_ERROR(NR_FILE_ERROR)
_ERROR(NR_FILE_NOT_FOUND)
_ERROR(NR_FILE_ERROR_IN_LINE)

_ERROR(NR_PROFILE_ERROR)
_ERROR(NR_PROFILE_NOT_FOUND)
_ERROR(NR_PROFILE_NOT_LOADED)
_ERROR(NR_PROFILE_NOT_EXISTS)
_ERROR(NR_PROFILE_ALREADY_EXISTS)

_ERROR(NR_EXTENSION_ERROR)
_ERROR(NR_EXTENSION_NOT_SUPPORTED)
  
_ERROR(NR_VFS_ERROR)
_ERROR(NR_VFS_ALREADY_OPEN)
_ERROR(NR_VFS_CANNOT_OPEN)
_ERROR(NR_VFS_CANNOT_CLOSE)
_ERROR(NR_VFS_IS_NOT_OPEN)
_ERROR(NR_VFS_FILE_NOT_FOUND)
_ERROR(NR_VFS_FILE_NOT_OPEN)
_ERROR(NR_VFS_FILE_END_REACHED)
_ERROR(NR_VFS_FILE_READ_ERROR)
_ERROR(NR_VFS_LINE_READ_ERROR)
_ERROR(NR_VFS_SCAN_READ_ERROR)
_ERROR(NR_VFS_SEEK_ERROR)

_ERROR(NR_APP_ERROR)

_ERROR(NR_LOG_ERROR)

_ERROR(NR_SETTINGS_ERROR)
_ERROR(NR_SETTINGS_VAR_ALREADY_REGISTERED)
_ERROR(NR_SETTINGS_VAR_NOT_REGISTERED)

_ERROR(NR_KERNEL_ERROR)
_ERROR(NR_KERNEL_NO_TASK_FOUND)

_ERROR(NR_TASK_ERROR)
_ERROR(NR_TASK_NOT_READY)

_ERROR(NR_CONSOLE_ERROR)
_ERROR(NR_CONSOLE_ITEM_ALREADY_EXISTS)


_ERROR(NR_TEX_ERROR)
_ERROR(NR_TEX_INVALID_FORMAT)
_ERROR(NR_TEX_NOT_CREATED_BEFORE)

_ERROR(NR_CLOCK_ERROR)
_ERROR(NR_CLOCK_OBSERVER_NOT_FOUND)
_ERROR(NR_CLOCK_OBSERVER_ALREADY_ADDED)

_ERROR(NR_FW_ERROR)
_ERROR(NR_FW_CANNOT_INITIALIZE)
_ERROR(NR_FW_FAILED_TO_RESIZE)
_ERROR(NR_FW_INVALID_RC_ID)
_ERROR(NR_FW_ALREADY_INIT)
_ERROR(NR_FW_NOT_INITIALIZED)

_ERROR(NR_RC_ERROR)
_ERROR(NR_RC_CANNOT_SETUP_PIXEL_FORMAT)

_ERROR(NR_ENGINE_ERROR)
_ERROR(NR_ENGINE_YOU_MUST_CREATE_ENGINE_BEFORE)

_ERROR(NR_RES_ERROR)
_ERROR(NR_RES_LOADER_ALREADY_EXISTS)
_ERROR(NR_RES_LOADER_NOT_REGISTERED)
_ERROR(NR_RES_ALREADY_EXISTS)
_ERROR(NR_RES_BAD_FILETYPE)
_ERROR(NR_RES_CAN_NOT_LOAD_EMPTY)
_ERROR(NR_RES_NOT_FOUND)
_ERROR(NR_RES_LOADER_NOT_EXISTS	)

		default:
			sprintf(str,"%x", res);
	}
	
#undef _ERROR
	return str;
}
#endif
