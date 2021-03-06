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
#include "ResourcePtr.h"
#include "Exception.h"
#include "ResourceSystem.h"

namespace nrEngine{
	
	//----------------------------------------------------------------------------------
	IResourcePtr::IResourcePtr(const IResourcePtr& resPtr){
		mHolder = resPtr.getResourceHolder();
	}

	//----------------------------------------------------------------------------------
	bool IResourcePtr::operator==(IResourcePtr& res) const{
		// check whenver given one is null
		if (res.isNull()){
			if (isNull())  return true;
			if (!isNull()) return false;
		}else{
			if (isNull())  return false;		
		}
		
		// check for holders
		ResourceHolder* A = getResourceHolder().get();
		ResourceHolder* B = res.getResourceHolder().get();
		if (A == B) return true;
		
		// At this point, both holders cannot be NULL
		if (A->getResource()->isResEmpty() || B->getResource()->isResEmpty()) return false;
		if (A->getResource() == B->getResource()) return true;
		
		return false;
	}

	
	//----------------------------------------------------------------------------------
	bool IResourcePtr::operator==(const IResource* p) const{
		
		if (isNull()){
			return (p == NULL);
		}
		
		return (getResourceHolder()->getResource() == p);
	}
		
	//----------------------------------------------------------------------------------
	bool IResourcePtr::operator!=(IResourcePtr& res) const{
			return !operator==(res);
	}

	//----------------------------------------------------------------------------------
	bool IResourcePtr::operator!=(const IResource* res) const{
			return !operator==(res);
	}
	
	//----------------------------------------------------------------------------------
	Result IResourcePtr::lockPure(){
		if (!isNull())
			if (!getResourceHolder()->lockPure())
				return RES_LOCK_STATE_STACK_IS_FULL;
		else
			return RES_PTR_IS_NULL;
			
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result IResourcePtr::unlockPure(){
		if (!isNull())
			getResourceHolder()->unlockPure();
		else
			return RES_PTR_IS_NULL;
		
		return OK;
	}
	
	//----------------------------------------------------------------------------------
	IResource* IResourcePtr::operator->() const{
		NR_ASSERT(mHolder.get() != NULL && "Holder does not contain valid data");
		return mHolder->getResource();
	}

	//----------------------------------------------------------------------------------
	IResource& IResourcePtr::operator*() const{
		NR_ASSERT(mHolder.get() != NULL && "Holder does not contain valid data");
		return *mHolder->getResource();
	}

		
};

