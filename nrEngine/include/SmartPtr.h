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

#ifndef __NR_SMART_PTR_H_
#define __NR_SMART_PTR_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

namespace nrEngine{

#define SharedPtr boost::shared_ptr

	/**
	 * Smart pointers we are using does not coded extra for nrEngine.
	 * Just use boost's implementation of this for our purpose.
	 *
	 * We use this extra definition to do less typing and to stay in the
	 * same namespace as the engine. So instead of typing
	 * boost::shared_ptr, we simply type SharedPtr.
	 * 
	 * \ingroup gp
	 **/
	/*template<class T>
	class _NRExport SharedPtr : public boost::shared_ptr<T>{
		public:
			SharedPtr() : boost::shared_ptr<T>(){}
			SharedPtr(T* t) : boost::shared_ptr<T>(t){}

			//! Copyconstructor
			SharedPtr(const SharedPtr& ptr) : boost::shared_ptr<T>(ptr) {
				
			}
			
	};*/

}; // end namespace

#endif
