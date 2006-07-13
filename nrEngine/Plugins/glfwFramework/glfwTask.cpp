
#include "glfwTask.h"

#define TASK_NAME "glfwTask"

//------------------------------------------------------------
glfwTask::glfwTask(Engine* root) : mEngine(root)
{
	setTaskName(TASK_NAME);

	NR_Log(Log::LOG_PLUGIN, "glfwBindigns: Initialize the glfw subsystem (OpenGL Framework)");

	// check for engine
	if (mEngine == NULL) throw(-1);

	// check if such a task already running
	if (mEngine->getKernel()->getTaskByName(TASK_NAME)) throw GLFW_TASK_ALREADY_RUNNING;

	// ok let's initialize the glfw subsystem
	if (glfwInit() == GL_FALSE) throw GLFW_CANNOT_INITIALIZE;

}

//------------------------------------------------------------
glfwTask::~glfwTask()
{
	NR_Log(Log::LOG_PLUGIN, "glfwBindigns: Terminate the glfw subsystem");
	glfwTerminate();
}

//------------------------------------------------------------
Result glfwTask::taskUpdate()
{
	return OK;
}

