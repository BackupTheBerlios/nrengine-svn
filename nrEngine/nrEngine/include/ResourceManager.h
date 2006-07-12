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


#ifndef _NR_RESOURCE_MANAGER_H_
#define _NR_RESOURCE_MANAGER_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ResourceSystem.h"

namespace nrEngine {

	//! General pointer/handle based resource management system
	/**
	* This class is a manger for every kind of resource used in our games.
	* Textures, sounds, meshes, ... - all things that can be loaded from disk
	* are resources managed by this module. You can derive your own classes
	* to write own storing algorithms for different kind of resources. This class
	* will use standard implementation and will try to be as efficient as possible.
	* This manager can index resources, look them up, load and destroy them, and
	* temporaly unload resources from memory to use only allowd size of memory.
	*
	* We will use a priority system to determine which resources can be unloaded
	* and for two resources of the same priority the most used will stay in memory.
	*
	* This code and idea was get from Game Programming Gems 4. <a href="http://www.ogre3d.org">
	* Ogre3D-Library</a> was also used to get an idea how to get the resource manager
	* working without using a whole library behind.
	*
	* The member functions createEmptyResource(), loadResource() are build by templates
	* that allows us to create new kind of resource by storing it under standrd nrIResourcePtr.
	* For example if you have a class A derived from nrCResource and this class is don't known
	* by our engine, so you can just load the resource object of this class by calling:
	* <code>nrResource.loadResource<A>(...)</code>. This allows us to create/load resources
	* of specified types and not just to create standard nrCResource object which do not always
	* can be casted to derived objects.
	*
	* <b>-</b> Meaning of resource groups:
	*		@par - Each resource can exists in a group.
	*			 - Each group has got it's unique names.
	*			 - You can access to whole groups either to single resources.
	*			 - E.g: you can split your level in patches and each resource in such a patch is
	*			   in patch's resource group. So if the player go to far away from such a patch,
	*			   it can be disabled or unloaded from the memory. You have now just to call one
	*			   unload function for the whole group.
	*			 - Groups must not be disjoint, so you can have same resource in different groups.
	*
	* \ingroup resource
	**/
	class _NRExport ResourceManager : public ISingleton<ResourceManager>{
	public:

		//! Constructor
		ResourceManager();

		/**
		* Release all used memory and also mark all loaded resources to unload. After
		* you calling the destructor of ResourceManager class you will
		* still be able to access to resources created by this manager, because
		* all of the resources pointers has got an reference counter.
		* As soon as you do not use the resource anymore and the manager is destroyed
		* the resource will be automaticly release used memory.
		*
		* But it is not a good way to use resources if the manager is not present.
		* You will get a WARNING in the log files if you do it this way, which will
		* help you to detect incostistencies and to remove resources completely before
		* you delete the resource manager.
		**/
		virtual ~ResourceManager();

		/**
		* Set amount of memory that can be used by the manager. Set this value to 80-90%
		* of your physical memory if you do not have any virtual memory in your system
		* available. The manager will try to stay in the budget you given for him.
		* Resources that are least used and has got lowest priority will be unloaded
		* to free the memory to stay in the given budget of memory.
		*
		* @param bytes Count of bytes which can be used for resource storing
		* @return either OK or error code.
		*
		* @note You can set this value to ca 80-90% of your memory available.
		*		 Before resource manager unload least used resources it will allocate
		*		 memory for the new resource to check how much it needs. So we still need
		*		 some free memory for this temporary allocation. So 80-90% are good
		*		 values for memory budget.
		*		 If you want to be very precisious you can set this value to memory
		*		 size minus biggest resource size. Which will use maximal available
		*		 memory space.
		**/
		Result setMemoryBudget(size_t bytes);


		/**
		* Get the memory budget resource manager have got.
		**/
		size_t	 getMemoryBudget() const;


		/**
		* Returns the usage of memory in bytes.
		**/
		size_t	 getMemoryUsage() const;


		/**
		* Here you can register any loader by the manager. Loader are used to load resources
		* from files/memory and to store resources in the memory. By registration of
		* a loader you have to specify his unique name. This name will be used to access
		* to stored loader (for example to remove it).
		*
		* @param name Unique loader name
		* @param loader Smart pointer containing the loader
		*
		* @return either OK or error code:
		*			- RES_LOADER_ALREADY_EXISTS
		*			- BAD_PARAMETERS
		*
		* @note If there is two loaders which do support loading of file of the same filetype,
		*		 so the behavior of getting appropriate loader is undefined.
		**/
		Result	registerLoader	(const ::std::string& name, ResourceLoader loader);


		/**
		* Removes the loader from the loader list. All bounded filetypes
		* registrations will also be removed. So after you calling this you have
		* to check whenever you still able to load filetypes you want to use.
		*
		* @param name Unique name of the loader
		* @return either OK or error code:
		*		- RES_LOADER_NOT_REGISTERED
		*		- RES_ERROR
		**/
		Result	removeLoader	(const ::std::string& name);


		/**
		* Return the loader by given filetype. If there is no loader which can load
		* such a filetype NULL will be returned
		* @param fileType File type for which one the loader should be found
		**/
		ResourceLoader getLoaderByFile	(const ::std::string& fileType);


		/**
		* Get a loader that support creating of resource instances of the given resource type
		* @param resType Unique name of the resource type
		* @return either NULL or resource loader
		**/
		ResourceLoader getLoaderByResource(const ::std::string& resType);


		/**
		* Return the loader by given name. If the loader with this name does not
		* exists, so NULL will be given back
		**/
		ResourceLoader getLoader			(const ::std::string& name);


		/**
		* This is a template function which will create an empty resource of the given
		* resource type "ResourceType". This function allows us to use general resource
		* manager class for all kind of resources with different types. So we need somewhere
		* a place where we can create objects of derived classes from Resource class.
		* And because you do not have to do it outside of this class and you do not have to
		* derive your own managers, you just specify here the type and it will be created.
		*
		* After you created a resource you will still not be able to use it. You have
		* to load a resource from a file. You have also to load it, because we are using
		* concept of empty resources. So loader has to get access on the resource to
		* initialize it with empty data.
		*
		* @param name Unique name of that resource. The name must be specified !!!
		* @param group Name of the group to which this resource belongs.
		* @param resourceType Unique type name of the resource type
		* @param params Here you can specify parameter which will be send to the loader/creator
		*				so that he can setup resource parameters. Default is NULL, so no parameters.
		* @note - If you want to store empty data in extra files or you want to debug
		* 		 resource by loading files which get your attention on it (pink texture,
		*		 big explosion sound, ...), you have to do it outside in the Resource
		*		 class. This manager does only manages the resource and is not responsible
		*		 for loading of resources from files.
		*
		*		- We have to specify a resource type name to this function. This will try
		*		 to find the appropriate loader, that can create instances of that type and will use it.
		*		 We need to do this in that way because we cannot use this function with
		*		 templates and we also do not want to derive more resource manager classes than
		*		 this one.
		**/
		IResourcePtr	createResource	(const ::std::string& name,
										const ::std::string& group,
										const ::std::string& resourceType,
										NameValuePairs* params = NULL);

		/**
		* Load a resource from a file. For loading of a resource the registered
		* loader will be used. The filetype is defined by the last
		* characters of the file name. If we can not find this characters
		* so the manual loader will be used if such one is specified. Manual loader
		* will also be used if it is specified and filetype is detected.
		* You can assign your resource to special group. The group names
		* has to be unique.
		*
		* If such a resource could not load, NULL will be returned.
		*
		* The loader will also be used to load empty resource for that resource type.
		* If the empty resource already loaded, it will not be loaded again.
		* Also we will use the loader to create an instance of the resource object for
		* the appropriate derived class.
		*
		* @param name Unique name fo the resource
		* @param group Group name to which this resource should be assigned
		* @param resourceType Unique type name of the resource
		* @param fileName File name of the file to be loaded
		* @param params Here you can specify parameter which will be send to the loader/creator
		*				so that he can setup resource parameters. Default is NULL, so no parameters.
		* @param manualLoader Loader which should be used if you want to overload
		*				all registered loader.
		*
		**/
		IResourcePtr	loadResource	(const ::std::string& name,
										const ::std::string& group,
										const ::std::string& resourceType,
										const ::std::string& fileName,
										NameValuePairs* params = NULL,
										ResourceLoader manualLoader = ResourceLoader());

		/**
		* This function will add a given resource to the resource management system.
		* From now one resource management get the rights for unloading/reloading and
		* removing resources from the memory.
		*
		* @param res  Resource pointer to the resource. (Be careful this is not a smart pointer)
		* @param name Unique name of the resource that should be used in the database
		* @param group Name of the group to which the resource belongs
		* @return either OK or error code
		*
		* @note The resource should already know which loader does loading
		*		 of it from files. So database will not assign any loader to
		*		 that resource. You should do it before calling this method.
		**/
		virtual Result		add(IResource* res, const ::std::string& name, const ::std::string& group = "");


		/**
		* This method is used to lock the resource with the given name for using.
		* Locking of pure resource means here, that we want now to access to real resource
		* either to empty resource. Because of our transparent functionality of changing
		* between real and empty resource (if real resource is unloaded), we must have a possibility
		* to access to real resources bypassing this changing mechanism. You will need this for
		* example if you want to change some properties of a resource (for example texture flags).
		* If your resource is currently unloaded from the memory and you will try to change properties,
		* so there would be now effect because settings get changed in empty resource which would
		* not change anything (Assumption: resources handles correctly).
		*
		* Assume the resource is not loaded and you want to access it.
		* <code>
		* 	resource->setPropertyOne(...);
		* </code>
		* Affect to empty resource, because the resource is unloaded.<br>
		* <code>
		* 	ResourceMgr.lockPureResource("resource");<br>
		* 	resource->setPropertyOne(...);<br>
		* 	ResourceMgr.unlockPureResource("resource");<br>
		* </code>
		* Affect to the resource you probably means. You will set the property of the real
		* resource and not the empty one.
		*
		* @param name Unique name of the resource
		* @return either OK or error code
		*
		* @note You have to decide whenever you want to lock your resource or just work as it is. It is safer
		* to do locking if you have something like write function and do not lock anything for read only functions.
		* e.g:
		* 	- ReadOnly by textures : getting of a ID, drawing, ...
		* 	- Write by texture: set new size, get name (it has also to be locked, because empty texture will return
		* 		a name acoording to empty texture), load, ...
		**/
		virtual Result		lockPure(const ::std::string& name);

		/**
		* Overloaded function to allow locking through pointer directly
		* @param res Pointer to the resource
		**/
		virtual Result		lockPure(IResourcePtr& res);

		/**
		* Overloaded function to allow locking through handle.
		* @param handle Unique handle of the resource
		**/
		virtual Result		lockPure(ResourceHandle& handle);

		/**
		* This function is complement to the lockPure(...) methods. This will unlock the
		* real resource from using
		* @param name Unique name of the resource to be unlocked
		**/
		virtual Result		unlockPure(const ::std::string& name);

		/**
		* Overloaded function to allow unlocking through pointer directly
		* @param res Pointer to the resource
		**/
		virtual Result		unlockPure(IResourcePtr& res);

		/**
		* Overloaded function to allow unlocking through handle.
		* @param handle Unique handle of the resource
		**/
		virtual Result		unlockPure(ResourceHandle& handle);


		/**
		* Unload the resource from the memory.
		* Each resource should know how to unload itself. The holder holding
		* the resource will now show to an empty resource.
		*
		* @param name Unique name of the resource
		* @return either OK or error code:
		*		- RES_NOT_FOUND
		**/
		virtual Result		unload(const ::std::string& name);

		//! @see unload(name)
		virtual Result		unload(IResourcePtr& res);

		//! @see unload(name)
		virtual Result		unload(ResourceHandle& handle);


		/**
		* Reload the resource, so it will now contain it's real data.
		* Call this function if want to get your resource back after
		* it was unloaded.
		*
		* @param name Unique name of the resource
		* @return either OK or error code:
		*		- RES_NOT_FOUND
		*
		* @note Reloading of resource will cost time. The resource will
		*		 try to access to it's file through the loader with which
		*		 one it was created. So try to implement very intellegent
		*		 algorithm to prevent reloading/unloading as much as possible
		**/
		virtual Result		reload(const ::std::string& name);

		//! @see reload(name)
		virtual Result		reload(IResourcePtr& res);


		//! @see reload(name)
		virtual Result		reload(ResourceHandle& handle);


		/**
		* This will remove the resource from the database.
		* After you removed the resource and would try to access to it through the
		* manager NULL will be returned.
		*
		* However if you remove the resource it is not just deleted from the memory
		* because of shared pointer system. So the parts of application which gets
		* pointers to it are still able to access it.
		*
		* @param name Unique name of the resource
		* @return either OK or error code:
		*		- RES_NOT_FOUND
		**/
		virtual Result		remove(const ::std::string& name);

		//! @see remove(name)
		virtual Result		remove(IResourcePtr& res);

		//! @see remove(name)
		virtual Result		remove(ResourceHandle& handle);


		/**
		* Get the pointer to a resource by it's name.
		* If there is no resource with this name, so NULL pointer will be returned
		*
		* @param name Unique name of the resource
		*
		* @note At now you need a time of O(2n), where n is number of resources.
		*		 Performance can became better by using of hash_map O(1)
		**/
		virtual IResourcePtr	getByName(const ::std::string& name);


		/**
		* Same as getByName(), but here you get the resource by handle.
		**/
		virtual IResourcePtr	getByHandle(const ResourceHandle& handle);


		/**
		* Unload all elements from the group.
		* @param group Unique name of the group
		* @return either OK or RES_GROUP_NOT_FOUND or an error code from unload(...) - method
		**/
		virtual Result		unloadGroup(const ::std::string& group);

		/**
		* Reload all elements in the group
		* @param group Unique name of the group
		* @return either OK or RES_GROUP_NOT_FOUND or an error code from reload(...) - method
		**/
		virtual Result		reloadGroup(const ::std::string& group);

		/**
		* Remove all elements in the group
		* @param group Unique name of the group
		* @return either OK or RES_GROUP_NOT_FOUND or an error code from remove(...) - method
		**/
		virtual Result		removeGroup(const ::std::string& group);



		/**
		* This method should be called by resource objects to notify the database, that
		* the resource was loaded.
		* @param handle Unique resource handle
		**/
		virtual void			_notifyResourceLoaded(ResourceHandle& handle);

		/**
		* This method should be called by resource objects to notify the database, that
		* the resource was unloaded.
		* @param handle Unique resource handle
		**/
		virtual void			_notifyResourceUnloaded(ResourceHandle& handle);

	protected:

		//------------------------------------------
		// Variables
		//------------------------------------------
		size_t		mMemBudget;
		size_t		mMemUsage;

		ResourceHandle	mLastHandle;

		typedef ::std::map< ::std::string, ResourceLoader> loader_map;

		loader_map	mLoader;


		typedef ::std::map<ResourceHandle, SharedPtr<ResourceHolder> > 	res_hdl_map;
		typedef ::std::map< ::std::string, ResourceHandle>						res_str_map;
		typedef ::std::map< ::std::string, ::std::list<ResourceHandle> >	 		res_grp_map;
		typedef ::std::map< ::std::string, SharedPtr<IResource> >		res_empty_map;

		res_hdl_map	mResource;
		res_str_map mResourceName;
		res_grp_map mResourceGroup;
		res_empty_map mEmptyResource;


		//------------------------------------------
		// Methods
		//------------------------------------------

		/**
		* Release all resources.
		**/
		void removeAllRes();

		/**
		* Remove all loaders from the manager
		**/
		void removeAllLoaders();


		/**
		* Check if we have now memory available.
		* If we need to remove some resources from the memory to get free place
		* so do it.
		**/
		virtual Result checkMemoryUsage();

		/**
		* This will remove the resource from the database.
		* After you removed the resource and would try to access to it through the
		* manager NULL will be returned.
		*
		* However if you remove the resource it is not just deleted from the memory
		* because of shared pointer system. So the parts of application, wich gets
		* pointers to it, are still able to access to it.
		*
		* @param resPtr Remove the resource whithin this pointer
		* @return either OK or error code:
		*		- RES_NOT_FOUND
		**/
		Result removeImpl(IResourcePtr& resPtr);


		/**
		* Get the appropriate holder for the given resource name.
		* This is a internal function, so it will return a pointer to a smart pointer
		* pointing to the holder. This is done because of performance reasons.
		* If nothing found NULL will be returned.
		**/
		SharedPtr<ResourceHolder>*	getHolderByName(const ::std::string& name);

		/**
		* Same as getHolderByName(...) but now get the holder through a handle
		**/
		SharedPtr<ResourceHolder>*	getHolderByHandle(const ResourceHandle& handle);


		/**
		* This function should create a empty resource and give a pointer to it back.
		* This is a virtual function which returns NULL in the base class. So if you
		* want to derive other resource manager classes and you want to have very
		* special creating function you should write out this function.
		*
		* If this method is returns NULL, so we will use the resource object
		* created createEmptyResource() function.
		* @see createEmptyResource()
		**/
		virtual IResource* createEmptyImpl(ResourceHandle hdl,
											const ::std::string& name,
											const ::std::string& group,
											const ::std::string& resourceType,
											NameValuePairs* params = NULL){return NULL;}

		/**
		* This function should load a resource from a file. This function is declared
		* as virtual so it can be overloaded through functions from dirived classes.
		* This is not needed but if you want to have your own loading functions, you
		* can do it here
		* @see loadResource()
		**/
		virtual IResource* loadResourceImpl(ResourceHandle hdl,
											const ::std::string& name,
											const ::std::string& group,
											const ::std::string& resourceType,
											const ::std::string& fileName,
											NameValuePairs* params = NULL,
											ResourceLoader manualLoader = ResourceLoader()){return NULL;}


		/**
		* This function will check if there is already an empty resource for the given resource
		* type and will create such one if it is not exists
		* @param resourceType Unique name of a resource type
		* @param empty Reference to smart pointer containing an empty resource after
		* @param loader Appropritate loader for that resource type
		**/
		virtual Result checkEmptyResource(const ::std::string resourceType,
											SharedPtr<IResource>& empty,
											ResourceLoader loader);


	};

};

#endif
