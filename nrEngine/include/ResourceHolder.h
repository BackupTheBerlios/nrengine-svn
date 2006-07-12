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


#ifndef _NR_RESOURCE_HOLDER_H_
#define _NR_RESOURCE_HOLDER_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

namespace nrEngine{
	
	/**
	* This constant defines the size of the lock stack. 
	* All locking states are stored in the stack, to allow nested lock/unlock calling.
	* \ingroup resource
	**/
	const int32 NR_RESOURCE_LOCK_STACK = 128;

		
	//! Interstage between manager and resources for more efficience and safety
	/**
	* This is a holder class which is holding a resource of different type.
	* Resource pointers are pointing to this holder, so by referencing the pointer
	* you do not have to ask the manager to get the resource, but directly to access
	* it through this holder. So actually resource manager manages holders and
	* not resources or pointers directly.
	*
	* We use such a system to be more efficient by dereferncing the resource pointer.
	* Other and more trivial way is to ask the manager for resource by giving him the
	* handle. This end in a look up of handle tables and returning the resource pointer.
	* If we use holders, pointers will directly access that holders and each holder
	* is showing to one resource. So we do not have to do any lookups.
	*
	* This class is working transparent to the whole resource management system, so
	* you actually do not have to know how this class works and you also do not have
	* to derive any classes from it.
	*
	* This system was get from Game Programming Gems 4, and was expanded to be more
	* flexible and more efficient.
	*
	* \ingroup resource
	**/
	class _NRExport ResourceHolder{
	public:
	
		/**
		* Remove the object from the memory and also delete the resource
		* which is holded by this holder. The destructor should be declared as
		* public so it can be used by smart pointers
		**/	
		~ResourceHolder();
	
	private:
	
								friend class ResourceManager;
		template <class ResType>friend class ResourcePtr;
								friend class IResourcePtr;
								
		/**
		* Create the object of this class
		**/
		ResourceHolder();
	
		//! Shared pointer that holds the resource	
		SharedPtr<IResource> mResource;
		
		//! Shared pointer that holds the empty resource object
		SharedPtr<IResource> mEmptyResource;
		
		//! Store number that represents how often the resource was in use
		uint64		countAccess;
		
		//! Store the status if real resources lock stack
		bool		mLockStack[NR_RESOURCE_LOCK_STACK];
		
		//! Current position in the stack
		int32		mLockStackTop;
		
		/**
		* Lock real resource for using. Locking has the effect that the getResource() method
		* will now return real resources also if it is unloaded.
		*
		* @return true if locking was successfull otherwise false
		* @note If the lock stack is full you are not able lock anymore
		**/
		bool lockPure();
		
		/**
		* Unlock the real resurce. After unlocking you have standard behavior about using
		* of empty resources. If resource is unloaded, so empty resource will be used.
		*
		* @return true if unlocking was successfull otherwise false
		* @note In complement of the locking, you are always able to unlock. 
		*		If the lock state stack is empty and you are unlocking the resource, so this
		*		do not affect anything.
		**/
		void unlockPure();
		
		/**
		* Reset the stored resource to a new value
		**/
		void resetRes(IResource* res);
	
		/**
		* Returns true if this holder has a pointer to a empty resource
		**/
		inline bool hasEmpty() const{
			return mEmptyResource.get() == NULL;
		}
	
		/**
		* Return the number of access to this resource
		**/
		inline uint64 getAccessCount() const{
			return countAccess;
		}
		
		/**
		* Each access to the resource will should call this function. Here 
		* we do work that has to be done if a resource was used.
		**/
		inline void touchResource(){
			// count up the access count variable
			countAccess ++;
		}
		
		/**
		* Check whenever the resource is currently locked
		**/
		inline bool isLocked(){
			if (mLockStackTop > 0) 
				return mLockStack[mLockStackTop - 1];
			else
				return false;
		}
		
		/**
		* This will bind this holder to specific empty resource.
		* After loading of a resource from a file the manager will
		* create/load the empty resource according to the type of the resource.
		* Then it will generate a name for it and will bind the holder to that
		* empty resource. So if you will next load a resource of the same type,
		* so holder's empty resource pointer will still point to the same one.
		**/
		void setEmptyResource(IResource* res);
		void setEmptyResource(SharedPtr<IResource> res);
	
		/**
		 * Returns the empty resource of the resource type holded by this holder.
		 **/
		IResource* getEmpty();
	
		
		/**
		* Get access to the stored resource.
		* We declare this function as private, so we cannot access to the resource
		* from outside either as from manager or a smart pointer to resources.
		* We need to do it in such a way, to prevent you from deleting the resources
		* from outside as from the resource management system.
		*
		* If the resource is temporary unloaded or if it is not exists, the holder will
		* give you the empty resource back, which still can be NULL or not
		*
		* Each call of getResource() method will count up the access number.
		**/
		IResource* getResource();
			
	};

};
#endif
