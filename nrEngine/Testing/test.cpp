
#define NR_APP_PROFILING

#include <nrEngine/nrEngine.h>

#include "../Plugins/glfwFramework/glfwBindings.h"

using namespace std;
using namespace nrEngine;

class A : public EventT<A>{
	public:
		A() : EventT<A>(Priority::FIRST) {}
		void doStuff() { return; printf ("This is A event!\n"); }
};

class B : public EventT<B>{
	public:
		B() : EventT<B>(Priority::NORMAL) {}
		void doStuff() { return; printf ("This is B event!\n"); }
};


class Actor1 : public EventActor {
	public:

		Actor1() : EventActor("actor1") {}

		void OnEvent(const EventChannel& channel, SharedPtr<Event> event)
		{
			printf("actor1 - ");

			if (event->same_as<A>())
				printf("A\n");
			else if (event->same_as<B>())
				printf("B\n");
			else
				printf("Not supported\n");

		}

};

class Actor2 : public EventActor {
	public:
		Actor2() : EventActor("actor2") {}

		void OnEvent(const EventChannel& channel, SharedPtr<Event> event)
		{
			if (event->same_as<KeyboardEvent>()){
				try{
					//SharedPtr<KeyboardPressEvent> a = event_shared_cast<KeyboardPressEvent>(event);
					printf("actor2 - KEY_PRESS\n");
				}catch(...){
					printf("actor2 - BAD CAST!\n");
				}
			}else{
				//SharedPtr<KeyboardPressEvent> a = event_shared_cast<KeyboardPressEvent>(event);
				printf("actor2 - Not detected!\n");
			}
		}

};

void test_plugins(Engine* root);
void test_scripts(Engine* root);
void test_events(Engine* root);

int main(){

	// initialize the engine
	Engine* root = new Engine();

	// first try to log, it shouldn't work
	NR_Log(Log::LOG_CONSOLE, "Das ist ein Log-Test\n");
	root->getLog()->setEcho(Log::LOG_ENGINE, Log::LOG_CONSOLE);
	root->getLog()->setEcho(Log::LOG_APP, Log::LOG_CONSOLE);
	root->getLog()->setLevel(Log::LL_DEBUG);

	// now initialize the logging
	root->initializeLog("./");
	root->initializeEngine();
	root->loadPlugin("/usr/local/lib/nrEngine/", "rdtscTimeSource.so", "HighPerformanceTimer");
	NR_Log(Log::LOG_APP, "Jetzt soll logging funktionieren");

	// update the engine stepwise
	NR_Log(Log::LOG_APP, "Jetzt updaten wir die Engine schrittweise");
	int i = 0;
	Clock::GetSingleton().setFrameWindow(1);
	Clock::GetSingleton().setSyncInterval(2000);

	// Test engine's components
	test_plugins(root);
	test_scripts(root);

	// now create a new channel
	EventManager::GetSingleton().createChannel("Channel1");
	test_events(root);

	// test for key events
	Actor2 keysActor;
	//ResourceManager::GetSingleton().loadResource("glfwBindings", "Plugins", "Plugin", "/usr/local/lib/nrEngine/glfwBindings.so");
	//EventManager::GetSingleton().createChannel("glfwTask");
	glfwBindings_Init(root);
	NR_ASSERT(keysActor.connect("glfwTask") == OK);

	// test main loop
	while (i < 2)
	{
		NR_ProfileBegin( "Main" );

		{
			NR_Profile("Update");
			test_events(root);

			root->updateEngine();
			i++;
		}


		// get current time
		float32 time = Clock::GetSingleton().getTime();
		NR_Log(Log::LOG_CONSOLE, "Zeit=%f", time);

		NR_ProfileEnd( "Main" );
	}

	Profiler::GetSingleton().logResults();

	// release the engine
	delete root;

	return 0;
}


//----------------------------------------------------------------------
// Test plugin system
//----------------------------------------------------------------------
void test_plugins(Engine* root){

	// load a new plugin as resource
	ResourceManager::GetSingleton().loadResource("TestPlugin", "Plugins", "Plugin", "/usr/local/lib/nrEngine/Plugin_Test.so");

	// get some extra info about the CPU
	ScriptResult res = ScriptEngine::GetSingleton().call("getCpuString");

	cout << res << endl;
}


class nothing{
	public:

		ScriptFunctionDef(tester);
};


ScriptFunctionDec(tester, nothing){

	printf("It is running!\n");

	return ScriptResult();
}


//----------------------------------------------------------------------
// Test engine's scripting system
//----------------------------------------------------------------------
void test_scripts(Engine* root){

	ScriptEngine::GetSingleton().add("test", ScriptFunctionName(tester, nothing));

	ScriptEngine::GetSingleton().call("test");


	//ScriptEngine::GetSingleton().del("test");


}


void test_events(Engine* root){

	// create events and actors
	Actor1 a1;
	//Actor2 a2;
	SharedPtr<A> eventA(new A());
	SharedPtr<B> eventB(new B());

	// now send this two events to the channel, no output should occurs
	NR_ASSERT(EventManager::GetSingleton().emit("Channel1", eventA) == OK);
	NR_ASSERT(EventManager::GetSingleton().emit("Channel1", eventB) == OK);

	// now connect new actors to the channel
	NR_ASSERT(a1.connect("No_Such_Channel") != OK);
	NR_ASSERT(a1.connect("Channel1") == OK);

	// send both events to the channel
	NR_ASSERT(EventManager::GetSingleton().emit("Channel1", eventA) == OK);
	NR_ASSERT(EventManager::GetSingleton().emit("Channel1", eventB) == OK);

	// now deliver all messages
	//NR_ASSERT(EventManager::GetSingleton().taskUpdate() == OK);

	// connect second actor and send both events again
	//NR_ASSERT(a2.connect("Channel1") == OK);

	// send both events to the channel
	NR_ASSERT(EventManager::GetSingleton().emit("Channel1", eventA) == OK);
	NR_ASSERT(EventManager::GetSingleton().emit("Channel1", eventB) == OK);

	// now deliver all messages
	NR_ASSERT(EventManager::GetSingleton().taskUpdate() == OK);

}

