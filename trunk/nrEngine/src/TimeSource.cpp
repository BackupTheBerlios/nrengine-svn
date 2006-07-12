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
#include "TimeSource.h"
#include "Log.h"

#include <time.h>
#if NR_PLATFORM != NR_PLATFORM_WIN32
#    include <sys/time.h>
#endif

namespace nrEngine{
		
	//------------------------------------------------------------------------
	TimeSource::TimeSource() : _currentTime(0), _syncTime(0)
	{
	
	}

	//------------------------------------------------------------------------
	TimeSource::~TimeSource()
	{
	}
		
	//------------------------------------------------------------------------
	float64 TimeSource::getSystemTime()
	{
		// do not use rdtsc for time calculation, just get time
		// by system calls
		timeval now;
		gettimeofday(&now, NULL);

		// some precalculation
		float64 a = (now.tv_sec) * 1000.0;
		float64 b = (now.tv_usec)/ 1000.0;
						
		// clculate the time in seconds
		return 0.001 * (a+b);
		
	}

	//------------------------------------------------------------------------
	float64 TimeSource::getTime()
	{
		// retrive time information
		timeval now;
		gettimeofday(&now, NULL);

		// some precalculation
		float64 a = (now.tv_sec - _startTime.tv_sec) * 1000.0;
		float64 b = (now.tv_usec- _startTime.tv_usec)/ 1000.0;
					
		// clculate the time in seconds
		_currentTime = 0.001 * (a+b) + _syncTime;
		
		// return it back
		return _currentTime;
	}

	//------------------------------------------------------------------------
	void TimeSource::reset()
	{
		gettimeofday(&_startTime, NULL);
	}

	//------------------------------------------------------------------------
	void TimeSource::sync()
	{
		_syncTime = _currentTime;
		reset();
	}
		
}; // end namespace	

