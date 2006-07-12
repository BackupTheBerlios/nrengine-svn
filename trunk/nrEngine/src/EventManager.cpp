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
#include "EventManager.h"
#include "Log.h"
#include "Profiler.h"


namespace nrEngine{
		
	//------------------------------------------------------------------------
	EventManager::EventManager(){
		setTaskName("EventSystem");
		
		NR_Log(Log::LOG_ENGINE, "EventManager: Initialize the event management system");

		// create default system wide channel
		createChannel(NR_DEFAULT_EVENT_CHANNEL);
	}
			
	//------------------------------------------------------------------------
	EventManager::~EventManager()
	{
		// clear the database, so all channels are deleted
		mChannelDb.clear();	
	}
		
	//------------------------------------------------------------------------
	Result EventManager::createChannel(const std::string& name)
	{
		// check first whenever such a channel is already in the database
		if (getChannel(name)) return EVENT_CHANNEL_EXISTS;

		// channel is not in the database, so create it and fill it's data
		SharedPtr<EventChannel> channel(new EventChannel(this, name));

		// push the channel into the database
		mChannelDb[name] = channel;
		NR_Log(Log::LOG_ENGINE, "EventManager: New channel \"%s\" created", name.c_str());
		
		// OK
		return OK;
	}

	//------------------------------------------------------------------------
	Result EventManager::removeChannel(const std::string& name)
	{
		// check first whenever such a channel is already in the database
		SharedPtr<EventChannel> channel = getChannel(name);
		if (!channel) return EVENT_CHANNEL_NOT_EXISTS;

		// disconnect all the actor from the channel
		channel->_disconnectAll();
		mChannelDb.erase(mChannelDb.find(name));
		
		// log info
		NR_Log(Log::LOG_ENGINE, "EventManager: Remove channel \"%s\"", name.c_str());
		
		// OK
		return OK;
	}

	//------------------------------------------------------------------------
	SharedPtr<EventChannel> EventManager::getChannel(const std::string& name)
	{
		// search for such an entry in the db
		ChannelDatabase::iterator it = mChannelDb.find(name);
		
		if (it == mChannelDb.end()) return SharedPtr<EventChannel>();

		return it->second;
	}
		
	//------------------------------------------------------------------------
	Result EventManager::taskUpdate()
	{
		// go through each channel and deliver the messages
		ChannelDatabase::iterator it = mChannelDb.begin();
		for (; it != mChannelDb.end(); it++)
			it->second->deliver();

		// ok
		return OK;
	}

	//------------------------------------------------------------------------
	Result EventManager::emit(const std::string& name, SharedPtr<Event> event)
	{
		// Profiling of the engine
		_nrEngineProfile("EventManager.emit");

		// if user want to send the message to all channels
		if (name.length() == 0){
			ChannelDatabase::iterator it = mChannelDb.begin();
			for (; it != mChannelDb.end(); it++)
				it->second->push(event);
			
		}else{
			// get the channel according to the name and emit the message
			SharedPtr<EventChannel> channel = getChannel(name);
			if (channel == NULL)
				return EVENT_CHANNEL_NOT_EXISTS;

			channel->push(event);
		}
		
		// ok
		return OK;
	}

	//------------------------------------------------------------------------
	Result EventManager::emitSystem(SharedPtr<Event> event)
	{
		return emit(NR_DEFAULT_EVENT_CHANNEL, event);
	}

}; // end namespace	

