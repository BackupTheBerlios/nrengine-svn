 /***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __BASE_DEFS_H_
#define __BASE_DEFS_H_


//------------------------------------------------------------------------------
//	Global includes
//------------------------------------------------------------------------------
#include <assert.h>
#include <nrEngine/nrEngine.h>
#include <Bindings/glfw/Binding.h>


//------------------------------------------------------------------------------
// We use this namespaces of engine and engine bindings
//------------------------------------------------------------------------------
//using namespace nrEngine;
//using namespace nrBinding;


//------------------------------------------------------------------------------
//	Main definitions
//------------------------------------------------------------------------------
#define VERSION_STRING "v0.1"
#define PROGRAMM_STRING "theGame"

#ifndef EPSILON
#define EPSILON 1e-7f
#endif

#ifndef PI
#define PI 3.14159265358979323846f
#endif

//------------------------------------------------------------------------------
//	Macros definitions
//------------------------------------------------------------------------------
#ifndef DEG2RAD
#define DEG2RAD 				(PI / 180.0f)
#endif

#ifndef RAD2DEG
#define RAD2DEG 				(180.0f / PI)
#endif

#ifndef TODEG
#define TODEG(x)    			((x * 180.0) / M_PI)
#endif

#ifndef TORAD
#define TORAD(x)    			((x / 180.0) * M_PI)
#endif

#define SAFE_DELETE(p)   		{ if((p)!=NULL) { delete (p); (p)=NULL; } }
#define SAFE_ARRAY_DELETE(p)   	{ if((p)!=NULL) { delete [] (p); (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)   	SAFE_ARRAY_DELETE(p)

#define LOG(args)			nrEngine::Log::GetSingleton().log args;


//------------------------------------------------------------------------------
//	Properties definitions
//------------------------------------------------------------------------------


#endif
