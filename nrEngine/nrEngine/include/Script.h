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


#ifndef _NR_SCRIPT_RESOURCE__H_
#define _NR_SCRIPT_RESOURCE__H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "IScript.h"

namespace nrEngine{


	//! Simple script object based on engine's simple script language
	/**
	 * This is a simple script language object. We use this language as engine's
	 * default one to load other more powerfull script languages. This languages
	 * could be found in plugins.
	 *
	 * Also simple scripts can be used to setup some variables or to write config
	 * files that will be used to setup the engine's environment. This config files
	 * could also contains plugins loading and file system setup. So actually this
	 * simple language is enough to write simple applications.
	 *
	 * The scripts have very simple syntax:
	 *  - each line is a script command
	 *  - the line is bypassed to the script engine (so commands must be registered or it will cause no effect)
	 *  - one command per one frame
	 *  - commands can be bound to a certain time (execute command at this time)
	 *  - time values could be either relative to the previous command or absolute(= relative to the start time of the script)
	 *  - nontimed commands are executed sequentially
	 *  - timed commands are executed quasi parallel to the nontimed
	 *  - if one script calls another one, so the new one runs in parallel
	 *
	 *
	 * Example of such a script (I called this timeline scripts):
	 *
	 *  seq_command param1 // execute seq_command sequantially with param1 as first argument
	 *  1.452 | cmd1 par1	// execute the command cmd1 with parameter par1 at the time 1.452s (after starting of the script)
	 *  1.693 | run script/script2.tml // execute script script/script2.tml after the time reaches 1.693
 	 *
 	 * NOTE: We represent each script as a task, so the scripts are running in parallel
	 *
	 * @see IScript
	 * \ingroup script
	**/
	class _NRExport Script : public IScript{
		public:

			//! Allocate memory and initilize simple script
			Script();

			//! Deallocate memory and release used data
			~Script();


			/**
			* Load simple script language from a string.
			* @copydoc IScript::loadFromString()
			**/
			Result loadFromString(const std::string& str);

			/**
			* @copydoc IScript::getLastError()
			**/
			std::string getLastError();

			/**
			* @copydoc IScript::getLastErrorLong()
			**/
			std::string getLastErrorLong();

			/**
			 * Execute the script completely. So the script will be
			 * executed until it finishes. This function will lock the execution
			 * of the engine while the script is running.
			 *
			 * NOTE: Timed commands used in nrScript would not been executed
			 * 		here. This is because of the time which will not be updated
			 * 		while the script is running. Updating the time, means either to
			 * 		update the clock or the kernel. This is not allowed by tasks,
			 * 		it means tasks can not update them self only kernel can do this.
			 * 
			 * 		So if you call execute() so only sequential commands will be
			 * 		executed!
			 *
			 * NOTE: Be carefull by using looped scripts. If there is a loop, so the
			 * 		script will also be executed in loop mode, so if you do not stop it
			 * 		somehow your application could not react anymore!!!
			 *		If the script is looped, so warnign will be printed in a log file!
			**/
			Result execute();

			/**
			* Execute one step from the script.
			**/
			Result step();

		private:

			//!  Parse the given string as nrScript language
			Result parse(const std::string&);

			//! Set certain parameter from the script
			bool setParameter(const std::string& param);

			//! Reset the sequentiall command fifo
			void resetCommandFifo();

			//! Reset the timed command fifo
			void resetTimedCommandFifo();

			//! Tokenize the given line into command and arguments
			void tokenize(const std::string& line, std::string& cmd, std::vector<std::string>& args);

			//! Reset the command lists (if return false, so the task is removed from kernel)
			bool reset();
			
			//! Script is valid
			bool mValid;

			//! Error in line
			int32 mLastLine;

			//! Each command has this type
			typedef struct _cmd{

				//! Timestamp ( = 0 -> command is not timed
				float32 timestamp;

				//! estimated starting time based on the global clock time
				float32 estimatedStart;

				//! current time of the command
				float32 time;

				//! function name
				std::string cmd;

				//! arguments of the command
				std::vector<std::string> args;

			} Command;

			//! Here we store our timed commands
			std::vector< Command > mTimedCommand;

			//! Here we store our sequentiall commands
			std::vector< Command > mCommand;

			//! Store command queue of waiting command ids
			std::list< int32 > mCommandFifo;

			//! Store timed commands sorted by their time in the queue
			std::list< int32 > mTimedCommandFifo;

			//! Script looping modes
			typedef enum _loopMode{
				NO_LOOP = 0,
				LOOP_COMMAND = 1 << 0,
				LOOP_TIMED_COMMAND = 1 << 1,
				LOOP = LOOP_COMMAND | LOOP_TIMED_COMMAND
			} LoopMode;

			//! Looping mode
			int32 mLoop;

			//! Current time of the script
			float32 mTime;

	};


};

#endif
