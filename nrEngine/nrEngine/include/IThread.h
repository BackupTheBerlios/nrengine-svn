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


#ifndef _NR_I_THREAD_H_
#define _NR_I_THREAD_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

namespace nrEngine{

	//! Abstract class to run a ITask in a thread controlled by Kernel
	/**
	 * IThread is a base class for each kin dof tasks, allows also parallel
	 * running of a task as a thread. Kernel tasks are derived from thi class
	 * so a task has also a possibility to run in a own thread. Kernel
	 * controls the creation and managing of threads. Threads communicate
	 * through the engine's default communication protocol.
	 *
	 * Writing a task as a thread for your application is not a big issue.
	 * You have to listen on kernel messages coming on the engine's default
	 * channel to be able to react on stop/suspend/resume messages. Kernel
	 * can not simply hold on a task. Because this can corrupt data if a task was
	 * performing any important calculations. Also this is not a good style.
	 * Kernel will provide a function to force a killing of a thread-task, but
	 * prevent using of this function.
	 *
	 * Instead of killing the running threads, kernel will send a message about
	 * stopping the thread. A thread-task should react on this event to
	 * gracefully stop the execution.
	 *
	 * The thread interface does provide neccessary functions to your tasks, so
	 * you do not have to think about managing the kernel messages by yourself.
	 * Instead of this the thread will call taskStop() method to indicate the task,
	 * that kernel want to stop the thread. Suspending the task will cause the kernel
	 * to let the according thread sleeping until the task is resumed. There is
	 * no other way how you can let the task to sleep.
	 *
	 * So how the thread interface cooperate with the kernel and the task interface.
	 * You write your task in the way, like if it were a simple task running
	 * sequentially in the kernel. Each ITask itnerface is a derived class from
	 * IThread but a thread portion of the code will only be active if you use the task
	 * as a thread for the kernel. By marking the task as a thread i.e. by adding
	 * the task to the kernel as a thread, you will activate the thread interface automatic.
	 * Kernel will start the thread and will call the taskInit() and taskStart() functions
	 * at the right moment. If the task is a thread, so it will call threadStart() instead
	 * which will call thread_taskStart() method of derived ITask interface. The method
	 * will call your taskStart() method, so you will not notice if there is a difference
	 * between starting a task as a task or as a thread.
	 *
	 * In each cycly the kernel does call taskUpdate() method for sequential tasks. For
	 * parallel tasks (threads) it does not call anything, because thread interface already
	 * running and it will automaticaly call the appropritate taskUpdate() method, so
	 * the task is getting updated. Also the thread base class does check if there is
	 * any messages from the kernel (i.e. sleep or stop). If so it will call appropriate
	 * methods in ITask interface.
	 *
	 * NOTE: The IThread interaface and Kernel does do all the job for you to manage
	 * themself as a threads and to let them run in parallel. The only one thing it
	 * can not manage for you is synchronisation. You have to worry about this by yourself
	 * by using of mutex locking mechanism.
	 *
	 * \ingroup kernel
	**/
	class _NRExport IThread{
		public:
			/**
			 * Virtual destructor so we are able to derive classes
			 * with overloaded methods.
			 **/
			virtual ~IThread();

		protected:

			/**
			 * Protected constructor, so only the ITask-Interface
			 * and friends are able to create an instance of this class.
			 * In this way we protect wrong usage of this interface.
			 **/
			IThread();

			/**
			 * Call this method if a thread goes into sleep mode. The derived
			 * task should reimplement this method to call the appropriate suspend
			 * method
			 **/
			virtual void _noticeSuspend() = 0;

			/**
			 * Call this method by waking up a thread. The derived task interface
			 * should call the appropritate resume method
			 **/
			virtual void _noticeResume() = 0;

			/**
			 * Thread does call this method in a main loop. The method
			 * should call the appropritate update method in ITask interface
			 **/
			virtual void _noticeUpdate() = 0;

			/**
			 * Call this method by stopping the execution of a thread. The underlying
			 * task has to remove all used data, because after calling of this method
			 * it will be destroyed
			 **/
			virtual void _noticeStop() = 0;

		private:

			//! Store here the thread instance
			boost::thread* 	mThread;

			/**
			 * This is a entry point for a thread. The method is
			 * static because it do not have to access any data
			 * of the objec titself. The given thread instance as a parameter
			 * will be updated and managed as a thread.
			 **/
			static void run(IThread* mythread);

			//! Kernel is a friend class
			friend class Kernel;

			/**
			 * Kernel does call this method if the appropriate task is running
			 * as a thread in the kernel. This method manage the derived class by calling appropriate
			 * virtual methods, which has to be reimplemented in the ITask interface.
			 **/
			void threadStart();

			/**
			 * Kernel does call this method, if a thread should stop.
			 **/
			void threadStop();

			/**
			 * Kernel call this method if a thread is going into sleep mode
			 **/
			void threadSuspend();

			/**
			 * Kernel call this method if a thread is goind to resume
			 **/
			void threadResume();

			//! Define thread states
			enum ThreadState{
				//! Thread is not running it is stopped
				THREAD_STOP,

				//! Thread does running
				THREAD_RUNNING,

				//! Thread is in sleep state (no action, but ready to wakeup)
				THREAD_SLEEPING,

				//! Wakeup the thread as soon as possible
				THREAD_NEXT_RESUME,

				//! Suspend the thread as soon as possible
				THREAD_NEXT_SUSPEND
			};

			//! This is a variable which will manage the thread state
			volatile ThreadState	mThreadState;

			//! Mutex to lock the data before use
			boost::mutex mMutex;

			//! Change a thread to new state, use mutex to lock the state
			void changeState(ThreadState newState);

	};

}; // end namespace
#endif	//_NR...
