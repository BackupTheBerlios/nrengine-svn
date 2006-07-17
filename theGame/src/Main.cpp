/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "App.h"

#include <iostream>
#include <string>

using namespace nrEngine;

int main(int argc, char *argv[]){

	// variables
	Result ret = OK;
	App::Instantiate();

	// Application Object
	ret = App::GetSingleton().init();
	if (ret != OK){return -1;}

	ret = App::GetSingleton().start();
	if (ret != OK){return -1;}

	ret = App::GetSingleton().loop();
	if (ret != OK){return -1;}

	ret = App::GetSingleton().close();
	if (ret != OK){return -1;}

	App::GetSingleton().Release();

	// OK
	return 0;

}
