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
#include "ITask.h"
#include "Log.h"

namespace nrEngine{

	//--------------------------------------------------------------------
	ITask::ITask(){
		_taskCanKill = false;
		_taskOrder = ORDER_NORMAL;
		_taskID = 0;
		_taskState = TASK_STOPPED;
		_orderChanged = false;
		_taskType = TASK_USER;
		_taskGraphColor = 0;
		setTaskName("");
	}

	//--------------------------------------------------------------------
	ITask::ITask(const ::std::string& name){
		_taskCanKill = false;
		_taskOrder = ORDER_NORMAL;
		_taskID = 0;
		_taskState = TASK_STOPPED;
		_orderChanged = false;
		_taskType = TASK_USER;
		_taskGraphColor = 0;
		strncpy(_taskName, name.c_str(), 63);
	}
	
	//--------------------------------------------------------------------
	ITask::~ITask(){

	}

	//--------------------------------------------------------------------
	bool ITask::operator <  (const ITask &t){
		return this->_taskOrder < t._taskOrder;
	}

	//--------------------------------------------------------------------
	bool ITask::operator == (const ITask &t){
		return this->_taskOrder == t._taskOrder;
	}

	//--------------------------------------------------------------------
	bool ITask::operator <= (const ITask &t){
		return (*this < t) || (*this == t);
	}

	//--------------------------------------------------------------------
	bool ITask::operator >  (const ITask &t){
		return !(*this <= t) ;
	}
	
	//--------------------------------------------------------------------
	bool ITask::operator >= (const ITask &t){
		return !(*this < t) || (*this == t);
	}

	//--------------------------------------------------------------------
	bool ITask::operator != (const ITask &t){
		return !(*this == t);
	}

	//--------------------------------------------------------------------
	Result ITask::taskOnResume(){
		return OK;
	}

	//--------------------------------------------------------------------
	Result ITask::taskOnSuspend(){
		return OK;
	}

	//--------------------------------------------------------------------
	void ITask::setTaskOrder(taskOrder order){
		_taskOrder = order;
		_orderChanged = true;
	}

	//--------------------------------------------------------------------
	taskOrder ITask::getTaskOrder() const{
		return _taskOrder;
	}

	//--------------------------------------------------------------------
	taskID ITask::getTaskID() const{
		return _taskID;
	}

	//--------------------------------------------------------------------
	taskType ITask::getTaskType() const{
		return _taskType;
	}

	//--------------------------------------------------------------------
	taskState ITask::getTaskState() const{
		return _taskState;
	}
	
	//--------------------------------------------------------------------
	void ITask::setTaskType(taskType type){
		this->_taskType = type;
	}

	//--------------------------------------------------------------------
	void ITask::setTaskID(taskID id){
		this->_taskID = id;
	}
	
	//--------------------------------------------------------------------
	void ITask::setTaskState(taskState state){
		this->_taskState = state;
	}

	//--------------------------------------------------------------------
	void ITask::setTaskName(const ::std::string& name){
		strncpy(_taskName, name.c_str(), 63);
	}
	
	//--------------------------------------------------------------------
	Result ITask::addDependency(taskID id)
	{
		_taskDependencies.push_back(id);

		NR_Log(Log::LOG_KERNEL, Log::LL_DEBUG, "Task %s depends now on task id=%i", taskGetName(), id);
		
		return OK;
	}

	//--------------------------------------------------------------------
	Result ITask::addDependency(const ITask& task)
	{
		return addDependency(task.getTaskID());
	}

	//--------------------------------------------------------------------
	Result ITask::addDependency(const ITask* pTask)
	{
		return addDependency(pTask->getTaskID());
	}

	
}; // end namespace

