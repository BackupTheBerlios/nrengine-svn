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
#include "Clock.h"

//----------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------
#define DEFAULT_OBSERVER_NAME "__no_name__"


namespace nrEngine{
	
	//------------------------------------------------------------------------
	Clock::Clock() : ISingletonTask<Clock>("SystemClock"){
		
		currentTime = 0;
		frameTime = 0;
		frameNumber = 0;
		
		sourceStartValue = 0;
		sourceLastValue = 0;
		
		useFixRate = false;
		fixFrameRate = 0;
		
		// add default frame time	
		setFrameWindow (7);
		frameTime = _getFilteredFrameDuration();
		
		timeSource.reset();

		// setup some other values
		nextSyncTime = 0;
		syncInterval = 0;
		
	}
		
	//------------------------------------------------------------------------
	Clock::~Clock(){
		taskStop();
	}
	
	//------------------------------------------------------------------------	
	void Clock::setTimeSource(SharedPtr<TimeSource> timeSource)
	{		
		this->timeSource = timeSource;
	
		if (timeSource != NULL){
			timeSource->reset();
			sourceStartValue = timeSource->getTime();
			sourceLastValue = sourceStartValue;
		}
	}
	
	//------------------------------------------------------------------------	
	void Clock::setSyncInterval(uint32 milliseconds)
	{
		this->syncInterval = milliseconds;
	}
	
	//------------------------------------------------------------------------
	bool Clock::isAnySourceBounded(){
		return (timeSource != NULL);
	}
	
	//------------------------------------------------------------------------
	SharedPtr<Timer> Clock::createTimer(){
	
		// create timer as observer
		SharedPtr<ITimeObserver> timer(
					new Timer(Clock::GetSingleton()),
					null_deleter());
		
		// add him to the list
		int32 id = addObserver(timer);
		
		if (id == 0){
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "Clock::createTimer(): Can not add timer to the observer list");
		}
		
		// set the id of the timer
		timer->_observerID = id;
		
		// return created timer
		return ::boost::dynamic_pointer_cast<Timer, ITimeObserver>(timer);
		
	}
	
	//------------------------------------------------------------------------
	Result Clock::taskStop(){
		timeSource.reset();
		
		for (uint32 i=0; i < observers.size(); i++){
			observers[i].reset();
		}
		observers.clear();
		
		return OK;
	}
	
	//------------------------------------------------------------------------
	Result Clock::taskUpdate(){

		// check whenever a time source is bound
		if (timeSource == NULL) return CLOCK_NO_TIME_SOURCE;
		
		// calculate exact frame duration time
		float64 exactFrameDuration = _getExactFrameDuration();
		_addToFrameHistory(exactFrameDuration);
		
		// get filtered frame time
		frameTime = _getFilteredFrameDuration();
		currentTime = timeSource->getTime();
		frameNumber ++;
			
		// get through the list of connected observers and notify them
		ObserverList::iterator it;
		for (it = observers.begin(); it != observers.end(); ++it){
			(*it)->notifyTimeObserver();
		}
		
		// check if we need to sync the clock
		if (syncInterval > 0 && nextSyncTime < currentTime)
		{
			timeSource->sync();
			nextSyncTime = currentTime + static_cast<float64>(syncInterval) / 1000.0;
		}

		// OK
		return OK;
	}
	
	//------------------------------------------------------------------------
	Result Clock::taskInit()
	{
		return OK;
	}
		
	//------------------------------------------------------------------------
	Result Clock::taskStart()
	{
		return OK;
	}
	
	//------------------------------------------------------------------------
	float64 Clock::getTime() const{
		return currentTime;
	}
	
	//------------------------------------------------------------------------
	float32 Clock::getFrameInterval() const{
		return (float32)frameTime;
	}
	
	//------------------------------------------------------------------------
	int32 Clock::getFrameNumber() const{
		return frameNumber;
	}
	
	//------------------------------------------------------------------------
	float32 Clock::getFrameRate() const{
		return 1.0f/(float32)(frameTime);
	}
		
	//------------------------------------------------------------------------
	int32 Clock::addObserver(SharedPtr<ITimeObserver> timeObserver){
		
		int32 id = 0;
		
		if (timeObserver != NULL){
			observers.push_back (timeObserver);
			id = observers.size();
			observerIDList[::std::string(DEFAULT_OBSERVER_NAME) 
										+ ::boost::lexical_cast< ::std::string >(id)] = id;
			observers[observers.size()-1]->_observerID = id;
		}
		
		return id;
	}
	
	//------------------------------------------------------------------------
	Result Clock::removeObserver(int32 observerID){
		
		int32 id = observerID - 1;
		if (id <= 0 || id+1 >= static_cast<int32>(observers.size())
				|| observerIDList.find(::std::string(DEFAULT_OBSERVER_NAME) + 
							::boost::lexical_cast< ::std::string >(observerID)) == observerIDList.end()){
			return CLOCK_OBSERVER_NOT_FOUND;
		}
		
		// remove it from the list
		observers.erase(observers.begin() + id);
		
		// also clean the map enty for this id
		observerIDList.erase(observerIDList.find( ::std::string(DEFAULT_OBSERVER_NAME) + 
				::boost::lexical_cast< ::std::string >(observerID) ));
				
		// OK
		return OK;
	}
		
	//------------------------------------------------------------------------
	Result Clock::addObserver(const ::std::string& obsName,
												SharedPtr<ITimeObserver> timeObserver){
								
	
		// check whenever such an observer already exists
		if (observerIDList[obsName]){
			return CLOCK_OBSERVER_ALREADY_ADDED;
		}
		
		// add observer	
		if (timeObserver != NULL && obsName.length() > 0){
			observers.push_back (timeObserver);
			observerIDList[obsName] = observers.size(); 
			observers[observers.size()-1]->_observerID = observers.size();
		}
		
		return OK;
					
	}
	
	//------------------------------------------------------------------------
	Result Clock::removeObserver(const ::std::string& obsName){
	
		if (observerIDList[obsName]){
			int32 id = observerIDList[obsName] - 1;
			observers.erase(observers.begin() + id);
		}else{
			return CLOCK_OBSERVER_NOT_FOUND;
		}
		
		return OK;
	}
	
	//------------------------------------------------------------------------
	void Clock::setFrameWindow(int32 frameCount, float32 defaultFrameTime){
		
		frameFilteringWindow = frameCount > 1 ? frameCount : 1;
		frameDefaultTime = defaultFrameTime;
		frameDurationHistory.clear ();
		frameDurationHistory.push_back(frameDefaultTime);
	}
	
	//------------------------------------------------------------------------
	void Clock::setFixFrameRate(bool setFixRate, float32 fixFrameRate){
		useFixRate = setFixRate;
		this->fixFrameRate = fixFrameRate;
	}
		
	//------------------------------------------------------------------------
	float64 Clock::_getExactFrameDuration (){
		
		if (useFixRate || timeSource == NULL){
			return 1.0f/(fixFrameRate);
		}
		
		float64 sourceTime = timeSource->getTime();
		float64 frameDuration = sourceTime - sourceLastValue;
		
		if (frameDuration > 0.200){
			frameDuration = frameDurationHistory.back();
		}else if (frameDuration < 0){
			frameDuration = 0;
		}
		
		sourceLastValue = sourceTime;
		
		return frameDuration;
		
	}
	
	//------------------------------------------------------------------------
	void Clock::_addToFrameHistory (float64 exactFrameDuration){
		frameDurationHistory.push_back (exactFrameDuration);
		if (frameDurationHistory.size () > (uint32) frameFilteringWindow)
			frameDurationHistory.pop_front ();
	}
	
	//------------------------------------------------------------------------
	float64 Clock::_getFilteredFrameDuration () const{
		
		if (useFixRate){
			return 1.0f/(fixFrameRate);
		}
		
		float64 totalFrameTime = 0;
	
		::std::deque<float64>::const_iterator it;
		for (it=frameDurationHistory.begin();it != frameDurationHistory.end(); ++it){
			totalFrameTime += *it;
		}
		
		return totalFrameTime/frameDurationHistory.size();
	}
	
}; // end namespace

#undef DEFAULT_OBSERVER_NAME
