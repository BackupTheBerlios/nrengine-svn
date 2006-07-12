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
#include "ResourceHolder.h"
#include "Exception.h"
#include "ResourceSystem.h"
#include "Log.h"

namespace nrEngine{
	
	//----------------------------------------------------------------------------------
	ResourceHolder::~ResourceHolder(){
		mResource.reset();
	}
		
	//----------------------------------------------------------------------------------
	ResourceHolder::ResourceHolder(): countAccess(0){
		
		// empty the lock stack
		for (int32 i=0; i < NR_RESOURCE_LOCK_STACK; i++)
			mLockStack[i] = false;
			
		mLockStackTop = 0;
	
	}
		
	//----------------------------------------------------------------------------------
	bool ResourceHolder::lockPure(){

		// check whenever we've got the maximum, so do not lock
		if (mLockStackTop >= NR_RESOURCE_LOCK_STACK){
			if (mResource.get()){
				NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, 
					"Can not lock %s anymore. Lock state stack is full!", mResource->getResName().c_str());	
			}else{
				NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, 
					"Can not lock anymore. Lock state stack is full!");
			}
			
			return false;
		}else{		
			// lock it
			mLockStack[mLockStackTop++] = true;
		}
		
		return true;
	}	
		
	//----------------------------------------------------------------------------------
	void ResourceHolder::unlockPure(){
	
		// check whenever we've got right top position
		if (mLockStackTop > 0){
			// unlock it
			mLockStack[--mLockStackTop] = false;
		}
	}
		
	//----------------------------------------------------------------------------------
	void ResourceHolder::resetRes(IResource* res){
		mResource.reset(res);
	}
	
	
	//----------------------------------------------------------------------------------
	void ResourceHolder::setEmptyResource(IResource* res){
		mEmptyResource.reset(res);
	}
	
	//----------------------------------------------------------------------------------
	void ResourceHolder::setEmptyResource(SharedPtr<IResource> res){
		mEmptyResource = res;
	}

	
	//----------------------------------------------------------------------------------
	IResource* ResourceHolder::getEmpty(){
		return mEmptyResource.get();
	}
	
		
	//----------------------------------------------------------------------------------
	IResource* ResourceHolder::getResource(){
		
		// touch the resource
		touchResource();
			
		// get resource only if it is exists and loaded or if it exists and locked
		if (mResource.get()!=NULL){
			if (mResource->isResLoaded() || isLocked()){
				return mResource.get();
			}
		}
		
		NR_ASSERT(getEmpty() != NULL && "Here empty resource should be defined");
		
		return getEmpty();
	}

};

