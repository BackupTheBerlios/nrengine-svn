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


#ifndef _NR_EVENT__H_
#define _NR_EVENT__H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Priority.h"
#include "Exception.h"

namespace nrEngine{
		
	//! Base untemplated class used for the event instancies
	/**
	 * Event is a base untemplated class.
	 * Each new type of event should be derived from the templated event class
	 * EventT, which does store some runtime type information about the event.
	 *
	 * \ingroup event
	 **/
	class _NRExport Event {
		public:
			
			/**
			* Cast a certain event to another one. As casting we use
			* dynamic_cast<T>() which will return 0 if casting
			* can not be done of this kind of objects.
			*
			* If casting could not be done, so an exception will be thrown, which
			* you have to catch.
			**/
			template<class T>
			static T* event_cast(Event* base) throw (Exception)
			{
				T* ptr = NULL;		
				try{
					ptr = dynamic_cast<T*>(base);
					if (ptr == NULL) throw(Result(EVENT_COULD_NOT_CAST));
					
				}catch (Result i){
					NR_EXCEPT(i,
							  "Casting from Event* to given type is not possible",
							  "Event::event_shared_cast<T>()");
				}
				return ptr;
			}

			/**
			 * Same as event_cast() but cast now between
			 * smart pointers.
			 **/
			template<class T>
			static SharedPtr<T> event_shared_cast(SharedPtr<Event> base)  throw (Exception)
			{
				SharedPtr<T> ptr;
				try
				{
					ptr = boost::dynamic_pointer_cast<T, Event>(base);
					if (ptr == NULL) throw(Result(EVENT_COULD_NOT_CAST));
					
				}catch (Result i){
					NR_EXCEPT(i,
							  "Casting from Event* to given type is not possible",
							  "Event::event_shared_cast<T>()");
				}
				return ptr;
			}
		
			/**
			 * Release used memory
			 **/
			virtual ~Event();

			/**
			 * Here we get a type name information about this event type.
			 **/
			const std::string& getTypeName();

			/**
			 * Get the priority of this event message.
			 **/
			const Priority& getPriority();

			/**
			 * Check whenever this class is of the same type
			 * as the given one. The function is templated,
			 * so you can check for every kind of classes
			 * if the type of an object the same on as given one.
			 **/
			template<class U> bool same_as()
			{
				// check if the typenames are the same
				return (mTypeName == typeid(U).name());
			}

		private:

			//! Type name of the class
			std::string mTypeName;

			//! Priority of the message
			Priority mPriority;

			//! Friend is a templated EventT class
			template<class T> friend class EventT;
			
			/**
			 * Create new instance of a base class Event.
			 *
			 * @param typeName Unique type name for this class.
			 *
			 * NOTE: You can not directly derive new classes from
			 * this one, because the constructor is private and can
			 * only be accessed from friend class EventT<>.
			 * We implemented it in this way to hide the internal
			 * runtime typechecking from the user.
			 **/
			Event(const std::string& typeName, Priority prior);

	};

	
	//! Events are messages used to communicate between application components
	/**
	 * An event is a message used to establish a communication between
	 * application components or between application and user interface.
	 * Our communication is done through certain communication channels.
	 * So each message which has to be readed by actors has to be sent to a
	 * channel where this actors are connected to.
	 *
	 * Derive your events if you want to implement a communication through
	 * event management system of nrEngine. You can derive your own event
	 * types by: <code> class NewEvent : public EventT<NewEvent> {} </code>
	 * 
	 * \ingroup event
	**/
	template<class T>
	class _NRExport EventT : public Event{
		public:

			/**
			 * Create a new event object.
			 * This will cause to create als Event class by specifing
			 * the typename and the priority to it.
			 **/
			EventT(Priority prior) : Event(typeid(T).name(), prior)
			{
	
			}


			/**
			 * Destructor of the EventT class
			 **/
			~EventT(){}
					
	};

}; // end namespace

#endif
