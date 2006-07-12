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
#include "Script.h"
#include "ScriptEngine.h"
#include "Clock.h"
#include "Log.h"

namespace nrEngine {

	//----------------------------------------------------------------------------------
	Script::Script() : IScript()
	{
		setResourceType ("nrScript");
		mLastLine = 0;
		mLoop = NO_LOOP;
		mTime = 0;
	}

	//----------------------------------------------------------------------------------
	Script::~Script()
	{
		unloadRes();
	}


	//----------------------------------------------------------------------------------
	Result Script::loadFromString(const std::string& str)
	{
		mContent = str;
		return parse(mContent);
	}

	//------------------------------------------------------------------------------
	Result Script::parse(const std::string& script){

		std::stringstream str(script);
		if (script.length() == 0 || !str.good()) return OK;

		// get linewise
		char buf[1024];
		std::string buffer;
		std::string::size_type pos;
		std::string line;
		int32 l = 0;

		while (!str.eof()){
			l ++;
			mLastLine = l;

			str.getline(buf, 1024);
			buffer = std::string(buf);

			// get the line without comments
			pos = buffer.find("//");
			if (pos == std::string::npos)
				line = buffer;
			else
				line = buffer.substr(0, pos);

			// if the line is empty, so get the next one
			if (line.length() == 0) continue;

			// check for script parameters
			pos = line.find('^');
			if (pos != std::string::npos){
				std::string param = NR_trim(line.substr(pos+1));
				if (!setParameter(param)){
					NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Script: Unknown local parameter %s in line %d", param.c_str(), l);
					return SCRIPT_PARSE_ERROR;
				}
				continue;
			}


			// check for the timestamp
			pos = line.find('|');
			Command cmd;
			cmd.timestamp = 0;
			cmd.estimatedStart = 0;

			// timestamp found, so do extra stuff with this kind of commands
			if(pos != std::string::npos ){

				// retrieve the timestamp and the command and store it into the command buffer
				std::string time = NR_trim(line.substr(0,pos));
				std::string command = line.substr(pos+1);

				// try to convert the time and add the command to the list
				try{
					cmd.timestamp = boost::lexical_cast<float32>(time);
					tokenize(command, cmd.cmd, cmd.args);
					mTimedCommand.push_back(cmd);
				}catch(...){
					NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "Script: Unknown syntax in %d\n", l);
					return SCRIPT_PARSE_ERROR;
				}

			// timestamp not found, so it is a simple command
			}else{
				tokenize(line, cmd.cmd, cmd.args);
				mCommand.push_back(cmd);
			}
		}

		// if we got any command so we are valid
		return OK;
	}

	//------------------------------------------------------------------------------
	void Script::tokenize(const std::string& str, std::string& cmd, std::vector<std::string>& tokens)
	{
		using namespace std;
		
		// tokenize on space characters
		const string delimiters = " ";
		
		// Skip delimiters at beginning.
		string::size_type lastPos = str.find_first_not_of(delimiters, 0);
		
		// Find first "non-delimiter".
		string::size_type pos     = str.find_first_of(delimiters, lastPos);
	
		while (string::npos != pos || string::npos != lastPos)
		{
			// Found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));

			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);

			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
		}

		// now the first element in the token list is the command name
		cmd = tokens[0];
		
	}

	//------------------------------------------------------------------------------
	bool Script::setParameter(const std::string& param){

		if (param == "loop")
			mLoop |= LOOP;
		else if (param == "loop_seq")
			mLoop |= LOOP_COMMAND;
		else if (param == "loop_timed")
			mLoop |= LOOP_TIMED_COMMAND;
		else
			return false;

		return true;
	}

	//------------------------------------------------------------------------------
	void Script::resetCommandFifo()
	{
		// the untimed command fifo is very simple
		// the commands are executed in the way they were found in the file
		mCommandFifo.clear();
		for (uint32 i=0; i < mCommand.size(); i++){
			mCommandFifo.push_front(i);
		}
	}

	//------------------------------------------------------------------------------
	void Script::resetTimedCommandFifo()
	{
		// the timed commands are also setted in the fifo
		// the estimated end time is also computed
		mTimedCommandFifo.clear();
		for (uint32 i=0; i < mTimedCommand.size(); i++){
			mTimedCommandFifo.push_front(i);
			mTimedCommand[i].estimatedStart = Clock::GetSingleton().getTime() + mTimedCommand[i].timestamp;
		}
	}

	//----------------------------------------------------------------------------------
	Result Script::step()
	{
		// ----- sequentiall commands -----
		if (mCommandFifo.size())
		{
			// get the id of the command
			int32 id = mCommandFifo.back();
			mCommandFifo.pop_back();

			// execute it
			ScriptEngine::GetSingleton().call(mCommand[id].cmd, mCommand[id].args);
		}

		// ----- timed commands (quasi parallel execution) -----
		// scan through the command fifo
		std::list<int32>::iterator it;
		for (it = mTimedCommandFifo.begin(); it != mTimedCommandFifo.end(); )
		{
			int32 id = *it;
			mTimedCommand[id].time = mTime;

			// if the estimated start time is reeached, so start the command and remove it from the queue
			if (mTimedCommand[id].estimatedStart < mTimedCommand[id].time)
			{
				// remove the command from the fifo
				it = mTimedCommandFifo.erase(it);

				// execute it
				ScriptEngine::GetSingleton().call(mCommand[id].cmd, mCommand[id].args);

			}else
				it ++;

		}

		// update time
		// if script is sleeping, so no update occurs
		mTime = Clock::GetSingleton().getTime();

		// reset the command list
		reset();

		return OK;
	}

	//----------------------------------------------------------------------------------
	Result Script::execute()
	{
		// check for looped script
		static bool warned = false;
		if (!warned && mLoop & LOOP_COMMAND){
			warned = true;
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "Script: %s - you are executing a looped script!!! Check this!!!", getResName().c_str());
		}

		// loop the content of the script if script should be looped
		bool bLoop = false;
		do {
			// we go through the whole command list and execute it.
			while (mCommandFifo.size() > 0)
			{
				// get the id of the command
				int32 id = mCommandFifo.back();
				mCommandFifo.pop_back();
	
				// execute it
				ScriptEngine::GetSingleton().call(mCommand[id].cmd, mCommand[id].args);
			}
	
			// reset the list and check if the task is removed from the kernel
			bLoop = reset();
			
		} while(bLoop && (mLoop & LOOP_COMMAND));
			
		return OK;
	}

	//----------------------------------------------------------------------------------
	bool Script::reset()
	{
		if (mCommandFifo.size() == 0 && (mLoop & LOOP_COMMAND)){
			resetCommandFifo();
		}else if (mTimedCommandFifo.size() == 0 && (mLoop & LOOP_TIMED_COMMAND)){
			resetTimedCommandFifo();
		}else if (mCommandFifo.size() == 0 && mTimedCommandFifo.size() == 0 && !(mLoop & LOOP_COMMAND) && !(mLoop & LOOP_TIMED_COMMAND)){
			Kernel::GetSingleton().RemoveTask(this->getTaskID());
			return false;
		}
		return true;
	}
	
	//----------------------------------------------------------------------------------
	std::string Script::getLastError()
	{
		return "";
	}

	//----------------------------------------------------------------------------------
	std::string Script::getLastErrorLong()
	{
		return "";
	}


	//----------------------------------------------------------------------------------
	// ----------------- Implementation of the empty script object ---------------------
	//----------------------------------------------------------------------------------



	//----------------------------------------------------------------------------------
	EmptyScript::EmptyScript()
	{

	}

	//----------------------------------------------------------------------------------
	EmptyScript::~EmptyScript()
	{

	}

	//----------------------------------------------------------------------------------
	Result EmptyScript::unloadRes()
	{
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result EmptyScript::reloadRes()
	{
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result EmptyScript::loadFromString(const std::string& str)
	{
		return OK;
	}

	//----------------------------------------------------------------------------------
	std::string EmptyScript::getLastError(){
		return std::string();
	}

	//----------------------------------------------------------------------------------
	std::string EmptyScript::getLastErrorLong(){
		return std::string();
	}

	//----------------------------------------------------------------------------------
	Result EmptyScript::execute(){
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result EmptyScript::step()
	{
		return OK;
	}

};

