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
#include "IThread.h"
#include "EventManager.h"
#include <boost/bind.hpp>

namespace nrEngine{

	//--------------------------------------------------------------------
	IThread::IThread()// : EventActor(std::string("Thread_") + boost::lexical_cast<std::string>(id))
	{
		mThread = NULL;
		mThreadState = THREAD_STOP;
	}

	//--------------------------------------------------------------------
	IThread::~IThread()
	{
		// delete the thread object if it is not empty
		if (mThread){
			delete mThread;
		}
	}

	//--------------------------------------------------------------------
	void IThread::threadStart()
	{
		// Check if we have already a thread created
		if (mThread)
		{
			NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "IThread: the appropriate thread is already running!");
			return;
		}
		NR_Log(Log::LOG_KERNEL, "IThread: Create thread and start it");

		// now create a thread and let it run
		mThread = new boost::thread(boost::bind(IThread::run, this));

	}


	//--------------------------------------------------------------------
	void IThread::threadStop()
	{
		changeState(THREAD_STOP);
		mThread->join();
	}

	//--------------------------------------------------------------------
	void IThread::threadSuspend()
	{
		changeState(THREAD_NEXT_SUSPEND);
	}

	//--------------------------------------------------------------------
	void IThread::threadResume()
	{
		changeState(THREAD_NEXT_RESUME);
	}

	//--------------------------------------------------------------------
	void IThread::changeState(ThreadState newState)
	{
		// first lock the mutex and then change the state
		try{
			boost::mutex::scoped_lock lock(mMutex);
			mThreadState = newState;
		}catch (boost::lock_error err) {}
	}

	//--------------------------------------------------------------------
	void IThread::run(IThread* mythread)
	{
		// start a thread, by setting it running state
		mythread->changeState(THREAD_RUNNING);

		// now loop the thread until some messages occurs
		bool run = true;
		while (run){

			// kernel requested to suspend the thread
			if (mythread->mThreadState == THREAD_NEXT_SUSPEND)
			{
				// notice about suspending and go into sleep mode
				mythread->changeState(THREAD_SLEEPING);
				mythread->_noticeSuspend();

			// kernel requested to resume the execution
			}else if (mythread->mThreadState == THREAD_NEXT_RESUME)
			{
				// notice about resuming the work and start it again
				mythread->changeState(THREAD_RUNNING);
				mythread->_noticeResume();

			// kernel does not requested anything, so run the task
			}else if (mythread->mThreadState == THREAD_RUNNING)
			{
				mythread->_noticeUpdate();
			}

			// check for the stop message, then stop the thread
			// this is a reading mutex, so do not have to lock it
			run = mythread->mThreadState != THREAD_STOP;
		}
		// notice to stop the underlying task
		mythread->_noticeStop();
	}

}; // end namespace

