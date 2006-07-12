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


#include "Kernel.h"
#include "Profiler.h"


namespace nrEngine {

	std::list< SharedPtr<ITask> >::iterator Kernel::_loopIterator;
	
	//-------------------------------------------------------------------------	
	Kernel::Kernel(){
		taskList.clear();
		pausedTaskList.clear();
		
		lastTaskID = 0;
		bTaskStarted = false;
		_bSystemTasksAccessable = false;
	}
		
	//-------------------------------------------------------------------------	
	Kernel::~Kernel(){
		StopExecution();
		
		taskList.clear();
		pausedTaskList.clear();

		// Log that kernel is down
		NR_Log(Log::LOG_KERNEL, "Kernel subsystem is down");
	}

	//-------------------------------------------------------------------------	
	void Kernel::lockSystemTasks()
	{
		_bSystemTasksAccessable = true;
	}
	
	//-------------------------------------------------------------------------	
	void Kernel::unlockSystemTasks()
	{
		_bSystemTasksAccessable = false;
	}
		
	//-------------------------------------------------------------------------	
	Result Kernel::OneTick()
	{
	
		// Profiling of the engine
		_nrEngineProfile("Kernel.OneTick");
		
		// start tasks if their are not started before
		if (!bTaskStarted)
			startTasks();
	
		// get iterator through our std::list
		PipelineIterator it;
	
		// we go always from teh first element in the list, because it is
		// the system root task 
		_loopStartCycle();
		while (_loopGetNextTask(taskList.begin(), it, 0) == OK){
		
			SharedPtr<ITask>& t=(*it);
	
			// update the task
			if (t.get()){
				if (!t->_taskCanKill && t->getTaskState() == TASK_RUNNING)
					t->taskUpdate();
			}
		}
		_loopEndCycle();
		
		PipelineIterator thisIt;
		taskID tempID;

		//loop again to remove dead tasks
		for( it = taskList.begin(); it != taskList.end();){
	
			SharedPtr<ITask> &t=(*it);
			thisIt = it;
			it ++;
	
			// check if task is valid
			if (t.get()){
				// kill task if we need this
				if(t->_taskCanKill){
					NR_Log(Log::LOG_KERNEL, "Stop task \"%s\" (id=%d) before removing",t->taskGetName(), t->getTaskID());
					t->taskStop();
					tempID = t->getTaskID();
					taskList.erase(thisIt);
					NR_Log(Log::LOG_KERNEL, "Task (id=%d) removed", tempID);
				}
	
				// check whenver order of the task was changed by outside
				if (t->_orderChanged){
					ChangeTaskOrder(t->getTaskID(), t->getTaskOrder());
				}
			}
		}
	
		return OK;
	}
	
	
	//-------------------------------------------------------------------------	
	void Kernel::startTasks(){
			
		// get iterator through our std::list
		PipelineIterator it;

		NR_Log(Log::LOG_KERNEL, "Create a system root task in the kernel");
		
		// firstly we add a dummy system task, which depends on all others.
		// This task do nothing, but depends on the rest.
		// This is needed to get all thing worked
		SharedPtr<ITask> root (new EmptyTask());
		root->setTaskName("KernelRoot");
		root->setTaskType(TASK_SYSTEM);

		// add all tasks as children to the root task
		for(it = taskList.begin(); it != taskList.end(); it++){
			root->addDependency((*it)->getTaskID());			
		}
		for(it = pausedTaskList.begin(); it != pausedTaskList.end(); it++){
			root->addDependency((*it)->getTaskID());
		}
		
		// Add the task as system task to the kernel
		lockSystemTasks();
			AddTask(root, ORDER_SYS_ROOT);
		unlockSystemTasks();
		
		NR_Log(Log::LOG_KERNEL, Log::LL_DEBUG, "Start all kernel tasks");
		
		// start all tasks, which are not running at now
		for(it = taskList.begin(); it != taskList.end(); it++){
			if ((*it)->getTaskState() == TASK_STOPPED){
				NR_Log(Log::LOG_KERNEL, "Start task \"%s\" with id=%d", (*it)->taskGetName(), (*it)->getTaskID());
	
				// start the task, if can not start so not add this task to the std::list
				if( (*it)->taskStart() != OK){
					NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "Cannot start the Task, because of Task-Internal Error");
					(*it)->setTaskState(TASK_STOPPED);
				}else{
					(*it)->setTaskState(TASK_RUNNING);
				}
			}
		}
	
		bTaskStarted = true;
	}

	//-------------------------------------------------------------------------	
	Result Kernel::StartTask(taskID id){

		try{
			SharedPtr<ITask> task = getTaskByID(id);
			
			if (!task) {
				NR_Log(Log::LOG_KERNEL, Log::LL_DEBUG, "Can not start task with id=%d, because such task does not exists", id);
				return KERNEL_NO_TASK_FOUND;
			}
			
			if (task->getTaskState() == TASK_STOPPED){
				NR_Log(Log::LOG_KERNEL, "Start task \"%s\" with id=%d", task->taskGetName(), task->getTaskID());
	
				// start the task, if can not start so not add this task to the std::list
				if( task->taskStart() != OK){
					NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "Cannot start the Task, because of Task-Internal Error");
					task->setTaskState(TASK_STOPPED);
				}else{
					task->setTaskState(TASK_RUNNING);
				}
			}else if (task->getTaskState() == TASK_PAUSED){
				return ResumeTask(id);
			}
			
		} catch (...){
			return UNKNOWN_ERROR;
		}
		
		// OK
		return OK;
	}
	

	//-------------------------------------------------------------------------	
	Result Kernel::Execute(){
		try{
			
			// Log that kernel is initialized
			NR_Log(Log::LOG_KERNEL, "Kernel subsystem is active, start main loop");
			
			// loop while we have tasks in our pipeline
			while (taskList.size()){
				OneTick();
			}
			
			NR_Log(Log::LOG_KERNEL, "Stop kernel main loop");
			
		} catch(...){
			return UNKNOWN_ERROR;
		}

		// ok
		return OK;
	
	}
	
	
	//-------------------------------------------------------------------------	
	taskID Kernel::AddTask (SharedPtr<ITask> t, taskOrder order){

		_nrEngineProfile("Kernel.AddTask");
		
		try {

			t->_taskOrder = order;
						
			NR_Log(Log::LOG_KERNEL, "Add Task \"%s\" at order = %d",
						t->taskGetName(), t.get(), int32(t->getTaskOrder()));
			
			// check whenever such task already exists
			std::list< SharedPtr<ITask> >::iterator it;
			for (it = taskList.begin(); it != taskList.end(); it++){
				if ((*it) == t || (*it)->getTaskID() == t->getTaskID() || strcmp((*it)->taskGetName(),t->taskGetName())==0){
					NR_Log(Log::LOG_KERNEL, "Found same task in task std::list !");
					return 0;
				}
			}
			
			for (it = pausedTaskList.begin(); it != pausedTaskList.end(); it++){
				if ((*it) == t || (*it)->getTaskID() == t->getTaskID() || strcmp((*it)->taskGetName(),t->taskGetName())==0){
					NR_Log(Log::LOG_KERNEL, "Found same task in paused task std::list !");
					return 0;
				}
			}

			// check if the given order number is valid
			if (t->getTaskOrder() <= ORDER_SYS_LAST && t->getTaskType() == TASK_USER)
			{
				NR_Log(Log::LOG_KERNEL, "User task are not allowed to work on system order numbers");
				return 0;
			}
			
			// init task and check his return code
			NR_Log(Log::LOG_KERNEL, "Init task \"%s\"", t->taskGetName(), t.get());
			if (t->taskInit() != OK){
				NR_Log(Log::LOG_KERNEL, "Cannot initalize Task because of Task internal error");
				return 0;
			}
			
			
			// find the place for our task according to his order
			for( it=taskList.begin(); it != taskList.end(); it++){
				SharedPtr<ITask> &comp = (*it);
				if (comp->getTaskOrder() >= t->getTaskOrder()) break;
			}
			
			// create new task id and add the task
			t->setTaskID(++lastTaskID);
			taskList.insert (it,t);
		
			NR_Log(Log::LOG_KERNEL, "Task \"%s\" was added TaskID=%d",t->taskGetName(), t.get(), t->getTaskID());
			
		} catch(...){
			return UNKNOWN_ERROR;
		}
	
		return t->getTaskID();
	}
	
	
	//-------------------------------------------------------------------------	
	Result Kernel::RemoveTask  (taskID id){
	
		try{
			
			NR_Log(Log::LOG_KERNEL, "Remove task with id=%d", id);
			
			// find the task 
			PipelineIterator it;
			
			// check whenever iterator is valid
			if (!_getTaskByID(id, it, TL_RUNNING | TL_SLEEPING)){
				NR_Log(Log::LOG_KERNEL, "No such Task (id=%d) found !!!", id);
				return KERNEL_NO_TASK_FOUND;
			}

			// check whenever we are allowed to remove the task
			if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM)
			{
				NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to remove this task!");
				return KERNEL_NO_RIGHTS;
				
			}else{
				// say task want to remove his self
				NR_Log(Log::LOG_KERNEL, "Prepare to die: \"%s\" (id=%d)", (*it)->taskGetName(), (*it)->getTaskID());
				(*it)->_taskCanKill = true;
			}
			
		} catch(...){
			return UNKNOWN_ERROR;
		}
	
		// OK
		return OK;
	}
	
	
	//-------------------------------------------------------------------------	
	Result Kernel::SuspendTask  (taskID id){
		
		try{
			
			NR_Log(Log::LOG_KERNEL, "Suspend task (id=%d)", id);
			
			// find the task 
			PipelineIterator it;
			
			// check whenever iterator is valid
			if (!_getTaskByID(id, it)){
				NR_Log(Log::LOG_KERNEL, "No task with id=%d found", id);
				return KERNEL_NO_TASK_FOUND;
			}else{
				SharedPtr<ITask> &t = (*it);
					
				if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM){
					NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to suspend this task!");
					return KERNEL_NO_RIGHTS;
				}else{

					// suspend task
					t->taskOnSuspend();
					t->setTaskState(TASK_PAUSED);
					
					// before removing the task from task std::list move it to paused std::list
					// so we can guarantee that task object will be held in memory
					pausedTaskList.push_back(t);
					taskList.erase(it);
					
					NR_Log(Log::LOG_KERNEL, "Task id=%d is sleeping now", id);
				}
			}
			
		} catch(...){
			return UNKNOWN_ERROR;
		}
	
		// OK
		return OK;
	}
	
	//-------------------------------------------------------------------------	
	Result Kernel::ResumeTask  (taskID id){
	
		try{ 
			
			NR_Log(Log::LOG_KERNEL, "Resume task (id=%d)", id);
			
			// find the task 
			PipelineIterator it;
			
			// check whenever iterator is valid
			if (!_getTaskByID(id, it, TL_SLEEPING)){
				NR_Log(Log::LOG_KERNEL, "No task with id=%d found", id);
				return KERNEL_NO_TASK_FOUND;
			}
		
			SharedPtr<ITask> &t = (*it);
			if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM){
				NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to resume this task!");
				return KERNEL_NO_RIGHTS;
			}else{
				
				// resume the task
				t->taskOnResume();
				t->setTaskState(TASK_RUNNING);
				
				//keep the order of priorities straight
				for( it = taskList.begin(); it != taskList.end(); it++){
					SharedPtr<ITask> &comp=(*it);
					if(comp->getTaskOrder() >= t->getTaskOrder()) break;
				}
				taskList.insert(it,t);
				
				// erase task from paused std::list. Therefor we have to find it in the std::list
				if (_getTaskByID(id, it, TL_SLEEPING)){
					pausedTaskList.erase(it);
				}
			}
				
		} catch(...){
			return UNKNOWN_ERROR;
		}
	
		// OK
		return OK;
	}
	
	//-------------------------------------------------------------------------	
	Result Kernel::StopExecution(){
		
		try{
			
			NR_Log(Log::LOG_KERNEL, "Stop the kernel subsystem");
			
			// iterate through all tasks and kill them
			for(PipelineIterator it = taskList.begin(); it != taskList.end(); it++){
				NR_Log(Log::LOG_KERNEL, "Prepare to die: \"%s\" (id=%d)", (*it)->taskGetName(), (*it)->getTaskID());
				(*it)->_taskCanKill=true;
			}
			
			// iterate also through all paused tasks and kill them also
			for(PipelineIterator it = pausedTaskList.begin(); it != pausedTaskList.end(); it++){
				NR_Log(Log::LOG_KERNEL, "Prepare to die: \"%s\" (id=%d)", (*it)->taskGetName(), (*it)->getTaskID());
				(*it)->_taskCanKill=true;
			}
			
			// Info if we do not have any tasks to kill
			if (!taskList.size() && !pausedTaskList.size())
				NR_Log(Log::LOG_KERNEL, "There is no more tasks to be killed !");
			
		} catch(...){
			return UNKNOWN_ERROR;
		}
	
		// OK
		return OK;
		
	}
	
	
	//-------------------------------------------------------------------------	
	Result Kernel::ChangeTaskOrder(taskID id, taskOrder order){
	
		try{
	
			NR_Log(Log::LOG_KERNEL, "Change order of task (id=%d) to %d", id, int32(order));
		
			// find the task 
			PipelineIterator it;

			// check whenever iterator is valid
			if (!_getTaskByID(id, it, TL_RUNNING | TL_SLEEPING)){
				NR_Log(Log::LOG_KERNEL, "No task with id=%d found", id);
				return KERNEL_NO_TASK_FOUND;
			}
		
			SharedPtr<ITask> &t = (*it);

			// check if the given order number is valid
			if (order <= ORDER_SYS_LAST && t->getTaskType() == TASK_USER)
			{
				NR_Log(Log::LOG_KERNEL, "User task are not allowed to work on system order numbers!");
				return KERNEL_NO_RIGHTS;
			}

			// check for task access rights
			if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM){
				NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to change the order of system task!");
				return KERNEL_NO_RIGHTS;
			}else{
		
				// change order of the task
				t->setTaskOrder(order);
				t->_orderChanged = false;
				
				// sort lists
				pausedTaskList.sort();
				taskList.sort();
			}
					
		} catch(...){
			return UNKNOWN_ERROR;
		}
			
		// OK
		return OK;
	}
	
	//-------------------------------------------------------------------------	
	bool Kernel::_getTaskByID(taskID id, PipelineIterator& jt, int32 useList){

		// Profiling of the engine
		_nrEngineProfile("Kernel._getTaskByID");
	
		// get std::list we want to search in
		std::list< SharedPtr<ITask> > *li = NULL;

		// search in normal list
		if ((useList & TL_RUNNING) == TL_RUNNING){
			li = &taskList;
			
			// iterate through elements and search for task with same id
			PipelineIterator it;
			for ( it = li->begin(); it != li->end(); it++){
				if ((*it)->getTaskID() == id){
					jt = it;
					return true;
				}			
			}
		}		

		// search in paused list
		if ((useList & TL_SLEEPING) == TL_SLEEPING){
			li = &pausedTaskList;
			
			// iterate through elements and search for task with same id
			PipelineIterator it;
			for ( it = li->begin(); it != li->end(); it++){
				if ((*it)->getTaskID() == id){
					jt = it;
					return true;
				}			
			}
			
		}
		
		// not found so return an end iterator
		return false; 	
	}
	
	//-------------------------------------------------------------------------	
	bool Kernel::_getTaskByName(const std::string& name, PipelineIterator& it, int32 useList){
	
		// Profiling of the engine
		_nrEngineProfile("Kernel._getTaskByName");
		
		// get std::list we want to search in
		std::list< SharedPtr<ITask> > *li = NULL;
		
		// search in normal list
		if ((useList & TL_RUNNING) == TL_RUNNING){
			li = &taskList;
			
			// iterate through elements and search for task with same id
			PipelineIterator jt;
			for ( jt = li->begin(); jt != li->end(); jt++){
				if ((*jt)->taskGetName() == name){
					it = jt;
					return true;
				}			
			}
		}

		// search in paused list
		if ((useList & TL_SLEEPING) == TL_SLEEPING){
			li = &pausedTaskList;
			
			// iterate through elements and search for task with same id
			PipelineIterator jt;
			for ( jt = li->begin(); jt != li->end(); jt++){
				if ((*jt)->taskGetName() == name){
					it = jt;
					return true;
				}			
			}
		}

		// not found so return end iterator
		return false;
	}
	
	//-------------------------------------------------------------------------	
	SharedPtr<ITask> Kernel::getTaskByID(taskID id){
		try{
			
			// find the task 
			PipelineIterator it;
			
			// check whenever iterator is valid
			if (!_getTaskByID(id, it, TL_RUNNING | TL_SLEEPING)){
				NR_Log(Log::LOG_KERNEL, "getTaskByID: No task with id=%d found", id);
				return SharedPtr<ITask>();
			}
		
			// check the rights
			if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM){
				NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to access to this task!");
				return SharedPtr<ITask>();
			}
			
			return *it;
			
		} catch(...){
			return SharedPtr<ITask>();
		}
	
	}
	
	
	//-------------------------------------------------------------------------	
	SharedPtr<ITask> Kernel::getTaskByName(const std::string& name){
		try{
			
			// find the task 
			PipelineIterator it;
			
			// check whenever iterator is valid
			if (!_getTaskByName(name, it, TL_RUNNING | TL_SLEEPING)){				
				NR_Log(Log::LOG_KERNEL, "getTaskByName: No task with name=%s found", name.c_str());
				return SharedPtr<ITask>();
			}

			// check the rights
			if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM){
				NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to access to this task!");
				return SharedPtr<ITask>();
			}
		
			return *it;
			
		} catch(...){
			return SharedPtr<ITask>();
		}
	}

	//-------------------------------------------------------------------------	
	Result Kernel::_loopStartCycle()
	{

		// Profiling of the engine
		_nrEngineProfile("Kernel._loopStartCycle");
				
		// iterate through the tasks and mark them as non visited
		_loopIterator = taskList.begin();
		while (_loopIterator != taskList.end())
		{
			(*_loopIterator)->_taskGraphColor = 0;
			_loopIterator ++;
		}
		
		_loopIterator = pausedTaskList.begin();
		while (_loopIterator != pausedTaskList.end())
		{
			(*_loopIterator)->_taskGraphColor = 0;
			_loopIterator ++;
		}

		
		_loopIterator = taskList.begin();
		
		return OK;
	}
	
	//-------------------------------------------------------------------------	
	Result Kernel::_loopGetNextTask(PipelineIterator it, PipelineIterator& result, int depth)
	{
		// Profiling of the engine
		_nrEngineProfile("Kernel._loopGetNextTask");

		// check whenever this is the end
		if (it == taskList.end()) return KERNEL_END_REACHED;
		
		// get the task in this iterator
		const SharedPtr<ITask>& t = *it;

		if (t->_taskGraphColor == 1) return KERNEL_CIRCULAR_DEPENDENCY;
		if (t->_taskGraphColor == 2) return KERNEL_END_REACHED;

		t->_taskGraphColor = 1;
		
		// now check if the task has got childrens
		if (t->_taskDependencies.size() > 0){

			// check 
			// do for each child call this function recursively
			for (uint32 i = 0; i < t->_taskDependencies.size(); i++){
				
				PipelineIterator jt;
				if (!_getTaskByID(t->_taskDependencies[i], jt, TL_RUNNING | TL_SLEEPING)) return KERNEL_TASK_MISSING;

				// check whenever the child was already visited
				if ((*jt)->_taskGraphColor != 2){
					Result ret = _loopGetNextTask(jt, result, depth+1);

					// we return OK only if we have not found any circularity
					// and we are on the top (root-node)
					if (ret != OK){
						t->_taskGraphColor = 0;
						if (depth == 0 && ret != KERNEL_CIRCULAR_DEPENDENCY) return OK;
						return ret;
					}
				}
			}

		// we do not have childs, so this is a leaf node
		}
		
		// all our chlds are visited, so return the node itself
		t->_taskGraphColor = 2;
		result = it;
		return KERNEL_LEAF_TASK;
				
	}
	
	//-------------------------------------------------------------------------	
	Result Kernel::_loopEndCycle()
	{
		return OK;
	}

}; //namespace nrEngine

