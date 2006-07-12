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

#ifndef __NR_ENGINE_CORE_H_
#define __NR_ENGINE_CORE_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ISingleton.h"
#include "Log.h"

namespace nrEngine{

	//! Core class of the engine. Used to create subsystems.
	/**
	* \par
	* This is our main class representing the engine itself. This class is
	* able to create and initialize particular elements of the engine's core.
	* So before you can use the engine you have to create an instance
	* of this class.
	*
	* \par
	* Engine's core class is declared as singleton.
	*
	* \ingroup engine
	**/
	class Engine : public ISingleton<Engine>{
		public:

			/**
			* Create an instance of the engine's core object
			**/
			Engine();

			/**
			* Delete the core object and try to deinitialize and to delete
			* all subcomponents of the engine
			**/
			~Engine();

			
			/**
			* Initialize the log component of the engine. You have to call
			* this function before you get using of engine's components. They all
			* will log their work information to appropriate log files, so the log
			* part of the engie has to be initialized before.<br>
			* If you do not initilaize the logging functionality, so no log
			* files will be written. No error will occurs.
			*
			* \param logPath Under this directory all log files will be created
			* \return false if error occurs or true otherwise
			**/
			bool initializeLog(const std::string& logPath);


			/**
			* Get a pointer to a log subsystem of the engine.
			* The pointer is always not equal to NULL. By initializing of
			* the log subsystem you just setup the log target.
			* So you can always log to the log-system without checking
			* whenever pointer is valid or not.
			**/
			Log* getLog() const { return _logger.get(); }


			/**
			* Release the engine and all used memory. The call of this function
			* will immidiately stop all kernel tasks and will stop the engine.<br>
			* Call this function if you want to quit the engine.
			**/
			void stopEngine();

			/**
			* "Gentlemans, start your engines!" - This is the entry point for
			* your application to start the engine. Starting of the engine will
			* also start a kernel, which is running system + application tasks.
			* If no more tasks is active, the kernel will be shutted down and engine
			* will stop.
			**/
			void runEngine();

			/**
			* If you do not want to run the engine in their own loop (\a startEngine() )
			* you can update the engine through this function. This function should be
			* called each frame. This will update the kernel tasks and all engine's
			* subsystems
			**/
			void updateEngine();

			/**
			* This method will initialize all engine's subsystems, that are essential
			* for engine's work. Please call this function after you have intialized
			* the logging, because there will be a lot of log information.
			*
			* \return true if all intialization are succeed, or false otherwise
			**/
			bool initializeEngine();
			
			/**
			* Return a pointer to the kernel subsystem of the engine. Our kernel
			* is normaly a singleton, so you can access to it, without this method.
			* 
			* Returned pointer is always valid.
			* @see Kernel
			**/
			Kernel* getKernel() { return _kernel.get(); }

			/**
			* Return a pointer to the underlying clock of the engine. The clock
			* is a singleton, so you can access it on another way.
			*
			* Returned pointer is always valid
			* @see Clock
			**/
			Clock* getClock()	{ return _clock.get(); }

			/**
			 * Load a certain plugin directly into the engine. The plugin will
			 * be loaded and the initialize subroutine will be called. If the plugin
			 * returns an invalid resulting code, so the plugin will be unloaded again.
			 *
			 * Use this function to load certain plugins needed for your application
			 * (i.e. ScriptLoader) and let them run with certain parameters, so you are
			 * able to use the engine as you need.
			 *
			 * @param path Path where the plugin can be founded
			 * @param file Filename relative to the path of the plugin
			 * @param name Name which will be then used to access the plugin later
			 *
			 * @return false if an error occurs
			 *
			 * NOTE: Check log files for more detailed error description
			 **/
			bool loadPlugin(const std::string& path, const std::string& file, const std::string& name);

			// Get a already loaded plugin
			//Plugin getPlugin(const std::string& name);
			
			
		private:
			::boost::scoped_ptr<Log>		_logger;
			::boost::scoped_ptr<Kernel>		_kernel;
			::boost::scoped_ptr<Clock>		_clock;
			::boost::scoped_ptr<Profiler>	_profiler;
			::boost::scoped_ptr<ResourceManager> _resmgr;
			::boost::scoped_ptr<ScriptEngine> _script;
			::boost::scoped_ptr<EventManager> _event;
			
	};
	
}; // end namespace


#endif
