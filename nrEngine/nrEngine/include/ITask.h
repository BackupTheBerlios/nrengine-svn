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


#ifndef _NR_I_TASK_H_
#define _NR_I_TASK_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Priority.h"


namespace nrEngine{

	/**
	* Each task is defined through it's unique ID-number. The numbers are used
	* to access task through the kernel.<br>
	* There is no tasks with ID 0, because this number is reserved as for no id.
	* \ingroup kernel
	**/
	typedef uint32 taskID;


	/**
	* Each task can be in one of this states. It can be either stopped, running or paused (sleeping)
	* \ingroup kernel
	**/
	typedef enum {
		//! Task is currently stopped. So it was either started nor paused
		TASK_STOPPED,

		//! Task is currently running, so updates of the task are done
		TASK_RUNNING,

		//! Task is sleeping. No updates but waiting on resume signal.
		TASK_PAUSED
	}taskState;

	/**
	* There are three types of tasks in our kernel subsystem. One is the engine's
	* own tasks, which are essential for engine working. Second are tasks which comes
	* from the engine, but could be shutted down, if user want it. And the last
	* one is user tasks defined through the application using nrEngine.
	* \ingroup kernel
	**/
	typedef enum {
		//! Root tasks could not be stopped by user, only engine can do this
		TASK_SYSTEM,

		//! Engine tasks are tasks defined through the engine, but they are accessable to the user
		//TASK_ENGINE,

		//! User tasks are task defined by the user
		TASK_USER
		
	} taskType;
	
	
	/**
	* Each task is in the kernel in specified order. This enum describes all order/queue
	* positions of any task in the kernel system
	* \ingroup kernel
	**/
	typedef enum {
		//! This number defines the a gap between two adjacent order numbers
		ORDER_STEP			= 32768,

		/**
		* This is a order position reserved for system tasks, that should realy run as first.
		* You can not add any user task on this order position.
		**/
		ORDER_SYS_ROOT		= 0,
		
		//! First task 
		ORDER_SYS_FIRST		= 1 * ORDER_STEP,

		//! Next after first
		ORDER_SYS_SECOND	= 2 * ORDER_STEP,
		
		//! Next after first
		ORDER_SYS_THIRD		= 3 * ORDER_STEP,
		
		//! Next after first
		ORDER_SYS_FOURTH	= 4 * ORDER_STEP,

		//! Next after first
		ORDER_SYS_FIVETH	= 5 * ORDER_STEP,
		
		//! Greatest order border for system task (only ordering)
		ORDER_SYS_LAST		= 6 * ORDER_STEP,
		
		//! Task will be updated as first
		ORDER_FIRST			= 7 * ORDER_STEP,
		
		//! Next after the first
		ORDER_ULTRA_HIGH	= 8 * ORDER_STEP,
		
		//! Next smaller order number to the ultra high value
		ORDER_VERY_HIGH		= 8 * ORDER_STEP,
		
		//! Next after very high
		ORDER_HIGH			= 10 * ORDER_STEP,
		
		//! Default order number (Center point)
		ORDER_NORMAL		= 11 * ORDER_STEP,
		
		//! Later update as of the normal position
		ORDER_LOW			= 12 * ORDER_STEP,
		
		//! Updating next after low
		ORDER_VERY_LOW		= 13 * ORDER_STEP,
		
		//! Ultra low order by comparison to the normal value
		ORDER_ULTRA_LOW		= 14 * ORDER_STEP,
		
		//! Task having this order will be updated as last
		ORDER_LAST			= 15 * ORDER_STEP

	} taskOrder;

	//! Each component of the engine/application does run as tasks in the Kernel
	/**
	* \par
	* 	ITask - is an interface for tasks of our kernel system. Kernel runs all
	* 	specified and added task in their queue order (think on OS). So ITask
	* 	Interface defines functions to be implemented by each task. 
	*
	* \par
	* 	There is a rule to understanding order numbers:
	*		- Each order-number is a number
	*		- Bigger number - task will be executed later
	*		- Between each defined default order-numbers there is ORDER_STEP numbers available
	*		- There is no guarantee that two or more tasks with same order would be
	*			executed in next cycle in same order
	*		- you can increase/decrease the order by substracting/adding of any number to any
	*			predefined order state
	*		- Task names must be unique
	*
	* \par
	*	Sometimes tasks depends on each other. So for example on task should be
	*	executed before another one. One way to solve this problem is to set the
	*	order number of each task manually. Other way is to use the dependencies
	*	infrastructure provided by task interface. You can setup task through it's
	*	ids to define tasks on which this one depends. Kernel will try to solve all
	*	dependencies and will bring all dependeable tasks in the right order according
	*	to their order numbers. If there is circular dependencies, so both tasks will not
	*	be added to the kernel's task list and an error code according to that problem will
	*	be given back.<br>
	*		- if task A depends on task B, so task A will be updated after the task B.
	*		- if task A depends on task B and order number of task A is greater than B,
	*			so B will be updated before A, but the ordering number will not be changed.
	*
	* \par
	*	Each task should support error handling routines. Tasks are running in the try-catch
	*	block. If kernel will catch any error by updating the task, the \a ITask::taskHandleError()
	*	function will be called. You have either to handle the error there or to pass the
	*	error back. Kernel will then send up the error along the function stack.<br>
	*	If the task couldn't handle the error it will be stopped and will be removed from
	*	the kernel task list. So each task object should be able to release all used memory
	*	also if there was an error before in the task update function.<br>
	*	This infrastructure allows us to catch up and to handle errors produced by the tasks.
	*	We can also remove tasks which couldn't handle their own errors. If you implement
	*	the task in the right way, then we can guarantee that no tasks would crash the kernel.
	*
	* \ingroup kernel
	**/
	class _NRExport ITask{
	public:

		/**
		* Define default variables. Set also default task order to ORDER_NORMAL
		* and define the current state of the task to TASK_STOPPED.
		**/
		ITask();

		/**
		* You are allowed to specify the task name as a parameter for the task.
		* Because no one can create directly the ITask instance, so only derived
		* classes can setup their names.
		**/
		ITask(const ::std::string& name);
		
		/**
		* Virtual destructor to allow to derive new classes from this one
		**/
		virtual ~ITask();

		/**
		* One task is smaller than another one if it's order number is smaller
		**/
		bool operator <  (const ITask &t);

		/**
		 * Two tasks are the same if their order number are equal or they are the same objects
		 **/
		bool operator == (const ITask &t);
		
		bool operator <= (const ITask &t);
		bool operator >  (const ITask &t);
		bool operator >= (const ITask &t);
		bool operator != (const ITask &t);

		/**
		* Initialization of a task.
		**/
		virtual Result taskInit() { return OK; }


		/**
		* Here task will be started.
		**/
		virtual Result taskStart() { return OK; }


		/**
		* In each cycle of our game loop this method will be called if task was added to our kernel.
		**/
		virtual Result taskUpdate() = 0;


		/**
		* Stop the task before task will be killed.
		**/
		virtual Result taskStop() { return OK; }


		/**
		* taskOnResume() will be executed on waiking up the task from sleeping.
		**/
		virtual Result taskOnResume();

		/**
		* Kernel will call this method if task will be aked to go for sleeping.
		**/
		virtual Result taskOnSuspend();

		/**
		* Task should return his name. Name must not be unique. We will need this to read
		* Log-Files in simple way.
		**/
		const char* taskGetName(){ return _taskName; }

		/**
		* Get order number of this task
		**/
		taskOrder getTaskOrder() const;

		/**
		* Get id number of the task
		**/
		taskID getTaskID() const;

		/**
		* Get the type of the task.
		**/
		taskType getTaskType() const;

		/**
		* Get the state in which the task is
		**/
		taskState getTaskState() const;
		
		/**
		* Add a new task id of a task on which one this task depends.
		*
		* @param id Unique ID of a task on which one this depends
		* @return either OK or:
		*		- 
		**/
		Result addDependency(taskID id);
		
		/**
		* Overloaded function that allows to add a dependency from the task itself
		**/
		Result addDependency(const ITask& task);
		
		/**
		* Overloded function that allows to add a dependency from the pointer on a task
		**/
		Result addDependency(const ITask* pTask);
		
		//! Kernel should have the full access to the task data. So kernel is our friend
		friend class Kernel;

		//! Engine should also get full access to running tasks, to allow setting up system tasks
		friend class Engine;
		
	private:
		bool 		_taskCanKill;		// we can kill this task in next system cycle
		taskState	_taskState;
		taskID 		_taskID;			// from kernel given task ID (unique)
		taskOrder 	_taskOrder;			// order number of our tasks
		taskType	_taskType;			// type of the task
		
		bool 		_orderChanged;
		char 		_taskName[64];
		
		//! Used by the kernel
		int32	_taskGraphColor;
		
		//! This vector does store all task id's on which one this depends
		std::vector<taskID>		_taskDependencies;

		//! Set the task type. This method is private, so only friends can set the type
		void setTaskType(taskType type);

		//! Only friends are allowed to set the id of the task
		void setTaskID(taskID id);
		
		//! Friends are also allowed to set the task state
		void setTaskState(taskState state);
		
		//! Set new order number for this task
		void setTaskOrder(taskOrder order);

	protected:
			
		//! Setup the name of the task
		void setTaskName(const ::std::string& name);
		
	};

	//! Empty task does not affect anything. It can helps to group tasks by making htem depends on this task
	/**
	* This class representing a simple task which does not do anything.
	* This is an empty task wich can be used as placeholder or root task containing
	* only children (task on which one it depends).<br>
	* This technique can help us to combine tasks in groups by using of this
	* empty tasks.
	* \ingroup kernel
	**/
	class _NRExport EmptyTask : public ITask{
	public:
		EmptyTask() : ITask() {}
		virtual ~EmptyTask() {}

		//! Do nothing just return OK
		virtual Result taskInit() 	{ return OK; }

		//! Do nothing just return OK
		virtual Result taskStart()	{ return OK; }

		//! Do nothing just return OK
		virtual Result taskUpdate() { return OK; }
		
		//! Do nothing just return OK
		virtual Result taskStop()	{ return OK; }
		
	};
	
}; // end namespace
#endif	//_NR...
