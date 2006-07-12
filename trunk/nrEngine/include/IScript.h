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


#ifndef _NR_SCRIPT_INTERFACE_RESOURCE__H_
#define _NR_SCRIPT_INTERFACE_RESOURCE__H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Resource.h"
#include "ITask.h"

/*!
 * \defgroup script Engine Scripting
 *
 * Scripting is our form of data driven design of our engine. Scripts
 * are also resources that are loaded through specified loader. Scripts are
 * used to program the engine from the outside of the source code. You are
 * able to program the engine in the runtime by executing the scripts through
 * a console provided by the engine. To understand what the scripts are good for
 * use appropriate literature from the internet.
 *
 * Our engine is designed to be very expandable and generic, so we are not use
 * specific kind of script language. nrEngine does only provide an interface
 * for a script classes that are implemented by plugins. The main idea of such
 * an interface is to create a binding between the script language and the engine.
 *
 * The plugins classes have to know how to bind the specific script language with
 * the script interface of the engine. The console will acquire a script object to
 * be used for the console scripting. This script object is provided by the plugins
 * so it uses that language which is implemented in the plugin.
 *
 * If there no plugins providing a scripting language so the default one is used.
 * The default scripting language does not support anything except of some function calls.
 * This very simple languge can be used to load another script language or
 * to setup some properties of the engine.
 **/
 
namespace nrEngine{
	

	//! Script interface for any kind of script languages
	/**
	* This is an interface for any kind of scripting language provided by the plugins.
	* This interface is used to executed strings typed in the console or readed from
	* the scripting files.
	*
	* We also define each script as an task in the kernel. So the script will be
	* updated in each tick by the kernel and will be unloaded from the task queue as soon
	* as the script ends his execution. Using of scripts as a task has the advantage, that
	* we are able to run scripts with a certain priority/order number. So the script_Task can
	* run before certain application task or after them.
	*
	* 
	* @see IResource, ITask
	* \ingroup script
	**/
 	class _NRExport IScript : public IResource, public ITask{
		public:
			
			/**
			* @copydoc IResource::unloadRes()
			**/
			virtual Result unloadRes();
			
			/**
			* @copydoc IResource::reloadRes()
			**/
			virtual Result reloadRes();

			/**
			 * Run one step from the script. This method will be automaticaly
			 * called from the engine's kernel.
			 **/
			virtual Result taskUpdate();

			/**
			* Load a script from string. The given string should contain
			* the script in the appropriate language, so it can be executed.
			*
			* @param str String containing the script
			* @return either OK or:
			* 		- SCRIPT_PARSE_ERROR
			**/
			virtual Result loadFromString(const std::string& str) = 0;
	
			/**
			* Get short description of the last error.
			**/
			virtual std::string getLastError() = 0;
	
			/**
			* Get long description of the last error in the script.
			**/
			virtual std::string getLastErrorLong() = 0;
	
			/**
			 * Execute the script. The script will be executed completely, before
			 * go back to the system run. Use step() instead to run the script stepwise.
			 **/
			virtual Result execute() = 0;

			/**
			 * Run one step from the script. This method will be called from taskUpdate()
			 * so the script is get updated stepwise.
			 **/
			virtual Result step() = 0;
			 
			
		protected:

			//! Here we store the whole script as a string
			std::string mContent;
			
	};

	
	//! Empty script object that just do nothing(idle).
	/**
	 * Empty script objects do nothing. They idle.
	 * \ingroup script
	 **/
	class _NRExport EmptyScript : public IScript{
	public:
	
		//! Constructor does not call anything
		EmptyScript();
		
		//! Release used memory
		~EmptyScript();

		//! Loading an empty script from file does not affect anything
		Result loadFromString(const std::string& str);

		//! There will be no errors in empty scripts
		std::string getLastError();

		//! No long description available in empty scripts
		std::string getLastErrorLong();

		//! Executing of empty script does return immideately
		Result execute();

		//! Do nothing
		Result step();
		
		//! Unload the empty resource
		Result unloadRes();
		
		//! Reload the empty resource
		Result reloadRes();

	};
	
};

#endif
