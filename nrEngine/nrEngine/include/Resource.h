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


#ifndef _NR_I_RESOURCE__H_
#define _NR_I_RESOURCE__H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ResourceSystem.h"

namespace nrEngine{

	//! General interface to hold any kind of resources
	/**
	* This is an interface which is describing how resource classes should looks like.
	* Derive your own classes to get resource management system working. Each
	* resource containing only once in the memory if it has the same name. So all
	* names of resources should be unique. Resource is not directly stored by the
	* manager but by the ResourceHolder. This holder just stores the resource
	* and is unique for each resource. Manager just manages such holders and
	* gives the resource pointers access to it.
	*
	* All derived classes must implement the behavior of resource objects correctly.
	* Correct behavior of resource objects is done if resources do nothing what can
	* change the empty resource behavior if they are empty. For example if you use texture
	* and user is setting some values to it and it is at the moment unloaded, so the method
	* will be called by the empty texture object. Empty texture object should stay empty and
	* therefor there should not be any changes. So texture object has to check whenever it is
	* an empty resource and should not do anything.
	*
	* The empty flag should be set by the loader, because only the loader does know
	* if and when a resource can be defined as empty. For example you have to load the empty
	* resource texture before you declare this resource as empty.
	*
	* \ingroup resource
	**/
	class _NRExport IResource{
	public:

		/**
		 * Create resource instance.
		 **/
		IResource();

		/**
		 * Virtual destructor, so you can derive your own classes from this one
		 **/
		virtual ~IResource();

		/**
		* Here the class should return cound of bytes reserved
		* by the resource. The size must not contain the size of the class itself.
		* For example: 32Bit Texture of 16x16 Pixel has the size: 16*16*4 = 1024 Bytes +
		* some more bytes which does class need to store himself in the memory.
		**/
		virtual std::size_t getResDataSize(){return mResDataSize;}


		/**
		* Return here the name of the resource type. This name will be used
		* to assign right empty resources to the resource pointer/holder objects.
		* E.g: "Texture", "Sound",  ...
		**/
		virtual const std::string& getResType() const { return mResType; }

		/**
		* Unload the resource. Each resource should know how to unload it from
		* the memory. You can also implement the ResourceLoader::unloadResource()
		* method to unload the resource and just call it from here if resource
		* loader does better know how to unload.
		**/
		virtual Result unloadRes() = 0;


		/**
		* Reload the resource. This function will call unloadRed method.
		* After this it calls the loader method loadResource and return back.
		* Overload this function to get other functionality.
		**/
		virtual Result reloadRes();

		/**
		* This will add the resource to the resource manager.
		*
		* @param manager Resource manager to which one the resource should be added
		* @param name Unique name of the resource
		* @param group Name of a group to which one this resource belongs.
		* @return either OK or error code.
		**/
		virtual Result addToResourceManager(ResourceManager& manager, const std::string& name, const std::string group = "");

		/**
		* Return true if the resource is marked as loaded. If any resource is loaded
		* so it contains in the memory and has it's full data there.
		**/
		NR_FORCEINLINE bool 				isResLoaded() const {return mResIsLoaded;}

		/**
		* Return handle of this resource
		**/
		NR_FORCEINLINE ResourceHandle		getResHandle() const {return mResHandle;}

		/**
		* Return group name to which one this resource belongs
		**/
		NR_FORCEINLINE const std::string&	getResGroup() const {return mResGroup;}

		/**
		* Get the name of the resource
		**/
		NR_FORCEINLINE const std::string&	getResName() const {return mResName;}

		/**
		* Get the file name from which one this resource can be restored or loaded
		**/
		NR_FORCEINLINE const std::string&	getResFileName() const {return mResFileName;}

		/**
		* Returns true if this resource is a empty resource. Empty resources are used
		* to replace normal resources if they are unloaded.
		**/
		NR_FORCEINLINE bool 				isResEmpty()	{return mResIsEmpty;}

		/**
		* Get the priority of this resource. Each resource can have a priority number
		* specifiyng the importance of this resource towards other resources.
		**/
		NR_FORCEINLINE Priority				getResPriority(){ return mResPriority; }

		/**
		* Specify the priority of a resource. You can freely specify priorities whereever you
		* wants. Because they are only useful by deleting the resources from the memory.
		**/
		NR_FORCEINLINE void					setResPriority(Priority p){mResPriority = p;}

	protected:
		friend class ResourceManager;
		friend class IResourceLoader;

		//! Shows whenever resource is loaded (is in the memory)
		bool mResIsLoaded;

		//! Resource's loader with which one it was created
		ResourceLoader	mResLoader;

		//! Handle of the resource given from manager
		ResourceHandle	mResHandle;

		//! Filename from which one this resource can be reloaded/loaded
		std::string mResFileName;

		//! If true so this is a empty resource
		bool mResIsEmpty;

		//! Resource manager to which one this resource belongs
		ResourceManager*		mParentManager;

		//! Count of bytes that this resource is occupies in the memory
		std::size_t		mResDataSize;

		//! priority of a resource object need to determine the importance
		Priority		mResPriority;

		//! Name of the resource type
		std::string		mResType;

		/**
		 * Set the resource type for this resource.
		 * Method is declared as protected, so only engine can do this.
		 *
		 * @param type Name of the resource type
		 **/
		void	setResourceType(const std::string& type) { mResType = type; }

	private:
		//! Group name to which one this resource own
		std::string mResGroup;

		//! Name of the resource
		std::string mResName;

	};

};

#endif
