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


#ifndef _NR_RESOURCE_LOADER_H_
#define _NR_RESOURCE_LOADER_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ResourceSystem.h"

namespace nrEngine{

	
	//! Resource loader interface for loading different resources
	/**
	* This is an interface which has to be implemented by each resource loader.
	* A resource loader is an object which can load different kind of resources
	* from different kind of files. For example there can be a image loader
	* which can load PNG, DDS, TGA, .. images and this one can be used to load
	* texture resources. In this case you can derive resource loader classes for
	* different filetypes from your texture resource loader class or just use
	* image loading functions for different types.
	*
	* Resource loader can be registered by the resource manager. After it is
	* registered it will be automaticly used to load files of different types.
	* If there is no loader registered for that type, so file will not be loaded,
	* error will occurs and empty resource will be used.
	*
	* Each loader has also to support creating/loading of empty resources.
	* By creating of an empty resource it's holder will not hold any resource.
	* After you loading the resource it will be initialized with the resource data
	* and with the empty resource.
	*
	* If you have files that are also resources but are not supported by the nrEngine
	* you can derive a loader class from this interface. Then register the object,
	* created from this derived class, by the resource manager and this class will
	* automaticly used for such kind of resources. This is something like a plugin
	* system on compiling layer/level.
	*
	* The resource loader has also to support creating of resource instances.
	* Because we are using only one resource manager for any
	* kind of resources, we have somewhere to create the instance with
	* <code>IResource* res = new Resource()</code>. We can not use templates
	* in the resource manager, because nothing will be compiled into the finale
	* library file. So you will not be able to use new resource types.
	* This loader also helps us to solve this problem by creating an instance of
	* such a resource object for our resource database.
	*
	* Each loader has a method which says to the manager what kind of resource types 
	* it supports. By creating of a resource, you should specify the type of to be 
	* created resource. Manager will look in the database, wich loader can create an instance
	* of such a type and will force such a loader to create an instance.
	*
	* @note File types are case sensitive. So *.png and *.PNG are different file types.
	*
	* \ingroup resource
	**/
	class _NRExport IResourceLoader{
	public:
	
		friend class ResourceManager;
		
		/**
		* Constructor
		**/
		IResourceLoader(){}
		
		/**
		* Virtual destructor such that we can derive more classes from this one
		**/
		virtual ~IResourceLoader(){}
	

		/**
		 * Call this function to initilize the loader. Usually initialization of the
		 * loader does declare supported file and resource types. So because this function
		 * is pure virtual it must overloaded by derived classes.
		 *
		 * This method should be called from the constructor, to declare supported types.
		 **/
		virtual Result initialize() = 0;
		
		
		/**
		* This method should create and load a resource from file. The resource does
		* know its filename. Each derived class should implement this method to
		* allow loading of resources.
		*
		* There is also a default implementation of this ethod in the interface.
		* It just checks whenever the given resource is supported by this loaded.
		* Call this method from derived classes, if you want to have this behaviour.
		* 
		* @param resource Pointer to resource created before. 
		* @return either OK or an error code
		**/
		virtual Result loadResource(IResource* resource);
	
		
		/**
		* Unload the given resource. We have declared this function in the loader,
		* so we can also access through default loader interface to such a method.
		* The semantics is the same as calling unloading method from the resource
		* or the resource manager.
		*
		* The main aim of declaring this function here is the ability for each resource
		* of calling the loader unloading method without casting it to
		* the appropriate loader type. 
		*
		* There is also an default implementation of this function in this interface.
		* It simply check if the given resource is supported by this loader and then call
		* the unload function of this resource. Call this method from derived classes
		* if you want to have this behaviour
		.
		* @param resource Pointer to resource to be unloaded
		* @return either OK or an error code
		**/
		virtual Result unloadResource(IResource* resource);

				
		/**
		* Creating of an empty resource object. An empty resource object can be used in
		* normal way but contains no data.
		*
		* @param resourceType Unique name of the resource type to be created
		* @return Instance of empty resource
		**/
		virtual IResource* createEmptyResource(const std::string& resourceType) = 0;
	
		
		/**
		* Create an instance of appropriate resource object. Like <code>new ResourceType()</code>
		*
		* @param resourceType Unique name of the resource type to be created
		* @param params Default is NULL, so no parameters. Specify here pairs of string that
		*				represents the parameters for the creating functions. The derived
		*				resource loader should know how to handle with them.
		* @return Instance of such a resource
		**/
		virtual IResource* createResourceInstance(const std::string& resourceType, NameValuePairs* params = NULL) const = 0;
		
		
		/**
		* This method should return a vector of strings containing information about
		* which kind of resource instances can this loader create. Each resource has it's
		* unique resource type name. So this vector contains such names.
		**/
		virtual const std::vector<std::string>& getSupportedResourceTypes(){return mSupportedResourceTypes;}
	
	
		/**
		* This method will say if this loader does support creating of resource of the given
		* resource type.
		* 
		* @param resourceType Unique name of the resource type
		* @return <b>true</b> if this loader can create instances of such a type, false otherwise
		**/
		virtual bool supportResourceType(const std::string& resourceType) const;
		
		/**
		* This function will return a vector containing information about supported filetypes.
		* It means that this loader can load each file of such a filetype.
		**/
		virtual const std::vector<std::string>& getSupportedFileTypes(){return mSupportedFileTypes;}
		
		/**
		* This method will say if this loader does support loading of resource of the given
		* file type.
		* 
		* @param fileType File type name 
		* @return <b>true</b> if this loader can load such files, false otherwise
		**/
		virtual bool supportFileType(const std::string& fileType) const ;
			
	protected:
		std::vector< std::string >		mSupportedResourceTypes;
		std::vector< std::string >		mSupportedFileTypes;
		
		/**
		* Internal function which must be called by all derived classes to 
		* setup all supported resource types.
		* @param name Unique name of supported resource type
		*/
		void declareSupportedResourceType(const std::string& name){
			mSupportedResourceTypes.push_back(name);
		}
		void declareSupportedResourceType(const char* name){
			mSupportedResourceTypes.push_back(std::string(name));
		}
		
		
		/**
		* Internal function which must be called by all derived classes to 
		* setup all supported file types.
		* @param name Unique name of file type which is supported by the derived loader
		*/
		void declareSupportedFileType(const std::string& name){
			mSupportedFileTypes.push_back(name);
		}
 		void declareSupportedFileType(const char* name){
			mSupportedFileTypes.push_back(std::string(name));
		}
		
		
		/**
		* Set the resource as empty. We need this funmction here, because we should be able to
		* change properties of resources from any loader.
		**/
		void setResourceEmpty(IResource* res, bool b);
	
	};

};

#endif
