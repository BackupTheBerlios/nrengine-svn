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


#ifndef _NR_EVENT_TYPE_H_
#define _NR_EVENT_TYPE_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Priority.h"

namespace nrEngine{
			
	//! This kind of function could create objects of new types
	boost::function<void*
	//! Event types are used for the built in simulated typesafety of events
	/**
	 * EventType is a class representing a type of a certain event.
	 * The types are always unique, it means there is no two different types
	 * with the same identifier. Because of weakness of C++ in providing
	 * of multimethods technique without big circumstances and without
	 * performance leak we decide to implement our own simulated typesafety on
	 * events.
	 *
	 * Using of this approach allows us to use runtime event types. This means
	 * we can create new event types while the application is running, without
	 * recompiling it.
	 * 
	 * \ingroup event
	**/
	template<class T>
	class _NRExport Type{
		public:

			/**
			 * Construct a new type with the given identifier (name).
			 * A First we check if such type already exists by calling an
			 * underlying typechecker which checks for a type
			 * with the same identifier. If there is a type so an exception will
			 * be thrown!
			 **/
			Type(const std::string& name);

			/**
			 * Remove a type from the typesystem, so there is no more
			 * objects of this type could be created.
			 **/
			virtual ~Type();

			/**
			 * Get a name of a type. The name is always unique, so there is
			 * no two different types with the same name
			 **/
			const std::string& getName() { return mName; }

		private:

			//! Name of the type is stored here
			std::string mName;
			
	};
	
}; // end namespace

#endif
