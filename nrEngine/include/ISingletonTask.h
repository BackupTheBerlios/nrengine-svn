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


#ifndef _NR_I_SINGLETON_TASK_H_
#define _NR_I_SINGLETON_TASK_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ISingleton.h"
#include "ITask.h"
#include "Kernel.h"


namespace nrEngine{

	//! Tasks can also be singleton objects, so this is a base class for such tasks
	/**
	* ISingletonTask combines the interface for ITask-class and functionality
	* of a singleton object. All derived classes will have to implement standard
	* ITask interface for tasking. ISingletonTask provides also couple of
	* usefull functions for each class implementing this interface.
	* \ingroup gp
	**/
	template<typename T>
	class _NRExport ISingletonTask : public ISingleton<T>, public ITask {
		
	public:

		//! Default Constructor
		ISingletonTask() : ISingleton<T>(), ITask(){}

		//! Constructor allowe dto specify a name
		ISingletonTask(const ::std::string& name) : ISingleton<T>(), ITask(name) {}
		
		/**
		* returns intance of singleton part
		**/
		virtual T* operator->(){
			return T::GetSingletonPtr();
		}
		
		/**
		* @return Casted smart pointer to @a ITask Interface
		**/
		static SharedPtr<ITask> GetTaskInterface(){
			return boost::dynamic_pointer_cast<ITask, T>(ISingleton<T>::GetSmartPtr());
		}
		
		/**
		* Add task to a given kernel.
		*
		* @param kernel object of CKernel-Interface
		* @param order  order number for the task
		* @return result provided by @a CKernel::AddTask() method
		**/
		virtual Result AddToKernel(Kernel& kernel, taskOrder order = ORDER_NORMAL){
			return kernel.AddTask(GetTaskInterface(), order);
		}
		
	};
	
}; // end namespace
#endif	//_NRCSINGLETON_H_
