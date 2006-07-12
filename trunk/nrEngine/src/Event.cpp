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
#include "Event.h"


namespace nrEngine{
		
	//------------------------------------------------------------------------
	Event::~Event()
	{

	}

	//------------------------------------------------------------------------
	const std::string& Event::getTypeName()
	{
		return mTypeName;
	}

	//------------------------------------------------------------------------
	const Priority& Event::getPriority()
	{
		return mPriority;
	}

	//------------------------------------------------------------------------
	Event::Event(const std::string& typeName, Priority prior)
				:mTypeName(typeName), mPriority(prior)
	{

	}


}; // end namespace	

