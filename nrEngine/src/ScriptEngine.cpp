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
#include "ScriptEngine.h"
#include "Log.h"

namespace nrEngine{

	//----------------------------------------------------------------------------------
	ScriptEngine::ScriptEngine(){

	}

	//----------------------------------------------------------------------------------
	ScriptEngine::~ScriptEngine(){

		// empty the database
		mDatabase.clear();

	}

	//----------------------------------------------------------------------------------
	Result ScriptEngine::add(const std::string& name, ScriptFunctor func, const std::vector<ScriptParam>& param)
	{
		// first check whenever such function already registered
		if (isRegistered(name))
			return SCRIPT_FUNCTION_REGISTERED;

		// now add the function to the database
		mDatabase[name].first = func;
		mDatabase[name].second = param;

		// some statistical information
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ScriptEngine: New function \"%s\" registered", name.c_str());

		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ScriptEngine::add(const std::string& name, ScriptFunctor func, const VarArg& v){

		// get the elements and store them in vector
		std::vector<ScriptParam> p;
		v.convert<ScriptParam>(p);

		// OK
		return add(name, func, p);
	}

	//----------------------------------------------------------------------------------
	Result ScriptEngine::del(const std::string& name)
	{
		// get the function
		FunctionDatabase::iterator it = mDatabase.find(name);
		if (it == mDatabase.end()) return SCRIPT_FUNCTION_NOT_REGISTERED;

		mDatabase.erase(it);

		// some statistical information
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ScriptEngine: Function \"%s\" was removed", name.c_str());

		return OK;
	}

	//----------------------------------------------------------------------------------
	ScriptResult ScriptEngine::call(const std::string& name, const std::vector<std::string>& args)
	{
		// if no such function was found
		FunctionDatabase::iterator f = get(name);
		if (f == mDatabase.end()){
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ScriptEngine: Function \"%s\" was not found!", name.c_str());
			return ScriptResult();
		}

		// call the function
		return ((*f).second).first(args, (*f).second.second);
	}

	//----------------------------------------------------------------------------------
	//ScriptResult ScriptEngine::parse(const std::string& cmdStr)
	//{
		// tokenize the string
	//
	//}

	//----------------------------------------------------------------------------------
	bool ScriptEngine::isRegistered(const std::string& name)
	{
		// search for that name in the database
		FunctionDatabase::const_iterator it = mDatabase.find(name);
		return it != mDatabase.end();
	}

	//----------------------------------------------------------------------------------
	ScriptEngine::FunctionDatabase::iterator ScriptEngine::get(const std::string& name)
	{
		// search for that name in the database
		FunctionDatabase::iterator it = mDatabase.find(name);

		return it;
	}

};

