
#include <nrEngine.h>
#include "glfwTask.h"

//---------------------------------------------------------
// Some globals
//---------------------------------------------------------
using namespace nrEngine;

Engine* 	mRoot = NULL;
taskID		mTaskId = 0;

//---------------------------------------------------------
// Initialize the plugin
//---------------------------------------------------------
extern "C" int plgInitialize(Engine* root)
{
	if (root == NULL){
		NR_Log(Log::LOG_PLUGIN, "glfwBindigns: You must give valid pointer to the engine");
		return -1;
	}

	mRoot = root;
	SharedPtr<glfwTask>	mTask;

	// initialize glfw task
	try{

		mTask.reset(new glfwTask(root));

	}catch(int i){

		if (i == GLFW_CANNOT_INITIALIZE){
			NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "glfwBindings: Initialization of glfw fails! glfwinit() returns GL_FALSE");
		}else if (i == GLFW_TASK_ALREADY_RUNNING) {
			NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "glfwBindings: You try to start the same plugin twice. glfwTask is already running");
		}

		return i;

	}

	// ok task is created now add it to the kernel
	mTaskId = root->getKernel()->AddTask(mTask, ORDER_LAST);

	// check for error code
	if (mTaskId == 0){
		NR_Log(Log::LOG_PLUGIN, Log::LL_ERROR, "glfwBindings: glfwTask could not been added to the kernel");
		return -1;
	}

	return 0;
}


//---------------------------------------------------------
// Get the engines version number for which this plugin was wrote
//---------------------------------------------------------
extern "C" unsigned int plgEngineVersion( void )
{
	return NR_createVersionInteger(0,0,5);
}


//---------------------------------------------------------
// Get full string according to the plugin version
//---------------------------------------------------------
extern "C" char* plgVersionString( void )
{
	return "OpenGL Framework Library plugin v0.1 for nrEngine";
}


//---------------------------------------------------------
// Return a string according to the given error code
//---------------------------------------------------------
extern "C" char* plgError(int err)
{
	if (err == 0){
		return "";
	}else if (err == GLFW_CANNOT_INITIALIZE){
		return "GLFW_CANNOT_INITIALIZE";
	}else if (err == GLFW_TASK_ALREADY_RUNNING){
		return "GLFW_TASK_ALREADY_RUNNING";
	}else{
		return "Unknown";
	}

}


//---------------------------------------------------------
// Release the plugin
//---------------------------------------------------------
extern "C" void plgRelease( void )
{
	// remove the task from the kernel
	mRoot->getKernel()->RemoveTask(mTaskId);

}
