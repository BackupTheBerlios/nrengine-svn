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

#ifndef __BINDINGS_PLUGIN_H_
#define __BINDINGS_PLUGIN_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

namespace nrEngine {

	//! Binding abstract classes for the engine
	/**
	 * Bindings are loadtime plugins provide to the engine
	 * new functionality. They do not linked within the engine library
	 * but with application written for the engine.
	 *
	 * This is an abstract class providing some default methods
	 * which has to be overwritten by the binding. Later on we could
	 * create a binding manager, so it could initialize the bindings
	 * by user wish.
	 *
	 * \ingroup plugin
	 **/
	class _NRExport Binding{
		public:

			//! Initialize the binding and its subsystems
			Binding() {}

			//! Release used memory and close binded subsystem
			virtual ~Binding() {}

			//! Get the name of the binding
			virtual const std::string& getName() = 0;

			//! Get a full name including authors, version,... strings of the bindings
			virtual const std::string& getFullName() = 0;

	};

}

//! Declare a new namespace
namespace nrBinding {
	// PLACEHOLDER for new kind of namespace
};

#endif

