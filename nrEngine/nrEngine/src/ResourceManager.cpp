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

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "ResourceManager.h"
#include "Log.h"

namespace nrEngine{

	//----------------------------------------------------------------------------------
	ResourceManager::ResourceManager(){
		mLastHandle = 1;
		mMemBudget = 0;
		mMemUsage = 0;
	}


	//----------------------------------------------------------------------------------
	ResourceManager::~ResourceManager(){

		// unload all resources
		removeAllRes();

		// remove all loaders
		removeAllLoaders();


	}

	//----------------------------------------------------------------------------------
	void ResourceManager::removeAllRes(){

		// iterate through each resource and remove it
		res_hdl_map::const_iterator it;

		for (it = mResource.begin(); it != mResource.end(); it++){
			ResourceHandle hdl = it->first;
			remove(hdl);
		}
		mResource.clear();


		// iterate through each empty resource and remove it
		res_empty_map::iterator jt;

		for (jt = mEmptyResource.begin(); jt != mEmptyResource.end(); jt++){
			SharedPtr<IResource>& res = jt->second;

			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceManager: Remove empty resource of type %s", res->getResType().c_str());
			res.reset();
			mEmptyResource.erase(jt);
		}
		mEmptyResource.clear();

	}

	//----------------------------------------------------------------------------------
	void ResourceManager::removeAllLoaders(){

		// iterate through all loaders
		loader_map::const_iterator it;

		for (it = mLoader.begin(); it != mLoader.end(); it++){
			removeLoader(it->first);
		}

	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::setMemoryBudget(size_t bytes){
		mMemBudget = bytes;
		return checkMemoryUsage();
	}


	//----------------------------------------------------------------------------------
	size_t ResourceManager::getMemoryBudget() const{
		return mMemBudget;
	}

	//----------------------------------------------------------------------------------
	size_t ResourceManager::getMemoryUsage() const{
		return mMemUsage;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::registerLoader(const ::std::string& name, ResourceLoader loader){

		// check whenver such a loader already exists
		if (mLoader.find(name) != mLoader.end()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: %s loader already registered", name.c_str());
			return RES_LOADER_ALREADY_EXISTS;
		}

		// check for bad parameters
		if (loader == NULL || name.length() == 0){
			return BAD_PARAMETERS;
		}

		NR_Log(Log::LOG_ENGINE, "ResourceManager: Register new resource loader %s", name.c_str());

		// register the loader
		mLoader[name] = loader;

		// Give some log information about supported filetypes and resource types
		try{
			::std::vector< ::std::string>::const_iterator it;
			::std::string strTemp;

			const ::std::vector< ::std::string>&	fileTypes = loader->getSupportedFileTypes();
			for (it = fileTypes.begin(); it != fileTypes.end(); it++){
				strTemp += (*it) + " ";
			}
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Found loader \"%s\" for file types *.%s", name.c_str(), strTemp.c_str());
			strTemp = "";

			const ::std::vector< ::std::string>&	resTypes = loader->getSupportedResourceTypes();
			for (it = resTypes.begin(); it != resTypes.end(); it++){
				strTemp += (*it) + " ";
			}
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Found loader \"%s\" for resource types %s", name.c_str(), strTemp.c_str());
		}catch(...){
			return BAD_PARAMETERS;
		}

		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::removeLoader(const ::std::string& name){

		// get id of the loader
		loader_map::const_iterator jt = mLoader.find(name);
		if (jt == mLoader.end()){
			return RES_LOADER_NOT_REGISTERED;
		}

		NR_Log(Log::LOG_ENGINE, "ResourceManager: Remove loader %s", name.c_str());

		mLoader.erase(name);

		return OK;
	}

	//----------------------------------------------------------------------------------
	ResourceLoader ResourceManager::getLoaderByFile(const ::std::string& fileType){

		// scan through all loaders and ask them if they do support this kind of file type
		loader_map::const_iterator it;

		for (it = mLoader.begin(); it != mLoader.end(); it++){
			if (it->second->supportFileType(fileType)){
				return it->second;
			}
		}

		// Give some log information
		NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ResourceManager: There is no loader for *.%s files", fileType.c_str());

		return ResourceLoader();
	}

	//----------------------------------------------------------------------------------
	ResourceLoader ResourceManager::getLoader(const ::std::string& name){

		loader_map::iterator it = mLoader.find(name);

		if (it == mLoader.end()){
			return ResourceLoader();
		}

		return mLoader[name];
	}

	//----------------------------------------------------------------------------------
	ResourceLoader ResourceManager::getLoaderByResource(const ::std::string& resType){

		// scan through all loaders and ask them if they do support this kind of file type
		loader_map::const_iterator it;

		for (it = mLoader.begin(); it != mLoader.end(); it++){
			if (it->second->supportResourceType(resType)){
				return it->second;
			}
		}

		return ResourceLoader();

	}

	//----------------------------------------------------------------------------------
	IResourcePtr ResourceManager::createResource
					(const ::std::string& name, const ::std::string& group, const ::std::string& resourceType, NameValuePairs* params)
	{

		// check whenever such a resource already exists
		IResourcePtr pRes = getByName(name);
		if (!pRes.isNull()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Creating of resource fails. Resource %s already exists", name.c_str());
			return pRes;
		}

		// Get appropriate loader/creator
		ResourceLoader creator = getLoaderByResource(resourceType);
		if (creator == NULL){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not create a resource %s because there is no loader for %s resource type", name.c_str(), resourceType.c_str());
			return IResourcePtr();
		}

		// create new handle
		ResourceHandle handle = mLastHandle++;
		NR_Log(Log::LOG_ENGINE, "ResourceManager: Create new empty resource %s, id=%d", name.c_str(), handle);

		// create resource through implicit creation function
		IResource* res = createEmptyImpl(handle, name, group, resourceType, params);

		if (res == NULL){
			res = creator->createResourceInstance(resourceType, params);
			if (res == NULL){
				NR_Log(Log::LOG_ENGINE, "ResourceManager: Resource instance could not be created. Abort.");
				return IResourcePtr();
			}
		}

		// check now if a empty resource of that type already exists and load it if not
		SharedPtr<IResource> empty;
		checkEmptyResource(resourceType, empty, creator);

		// create a holder for that resource
		SharedPtr<ResourceHolder> holder(new ResourceHolder());
		holder->resetRes(res);
		holder->setEmptyResource(empty);

		// store the resource in database
		mResourceGroup[group].push_back(handle);
		mResource[handle] = holder;
		mResourceName[name] = handle;

		res->mResGroup = group;
		res->mResName = name;
		res->mResHandle = handle;
		res->mResLoader = creator;
		res->mParentManager = this;

		// check for memory usage
		mMemUsage += res->getResDataSize();
		Result ret = checkMemoryUsage();

		if (ret != OK){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Could not rearrange memory");
			return IResourcePtr();
		}

		// return a pointer to that resource
		return IResourcePtr(holder);
	}

	//----------------------------------------------------------------------------------
	IResourcePtr ResourceManager::loadResource(
								const ::std::string& name,
								const ::std::string& group,
								const ::std::string& resourceType,
								const ::std::string& fileName,
								NameValuePairs* params,
								ResourceLoader loader){

		// check whenever right parameters are specified
		NR_ASSERT(name.length() > 0  && fileName.length() > 0 && resourceType.length() > 0);

		::std::string type = "";
		ResourceHandle handle;
		IResource* res = NULL;
		bool isManualSpecified = (loader != NULL);

		// get appropriate loader for the specified resource type
		if (resourceType.length() > 0){
			loader = getLoaderByResource(resourceType);
			if (loader == NULL){
				NR_Log(Log::LOG_ENGINE, "ResourceManager::loadResource(): Can not create a resource %s because there is no loader for %s resource type", name.c_str(), resourceType.c_str());
				return IResourcePtr();
			}
		}

		// check whenever such a resource already exists
		ResourcePtr<IResource> pRes = getByName(name);
		if (!pRes.isNull()){
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ResourceManager: Resource %s already loaded", name.c_str());
			pRes->unloadRes();
		}else{

			// detect the file type by reading out it's last characters
			for (uint32 i = fileName.length()-1; i >= 0; i--){
				if (fileName[i] == '.'){
					break;
				}
				type = fileName[i] + type;
			}

			if (type.length() == 0 && loader == NULL){
				NR_Log(Log::LOG_ENGINE, "ResourceManager: Could not detect filetype of %s", fileName.c_str());
				return IResourcePtr();
			}
		}
		
		// get appropriate loader for that filetype
		if (!isManualSpecified){
			loader = getLoaderByFile(type);
		}
		if (loader == NULL){
			NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "ResourceManager: Loader for *.%s was not found or no manual loader was specified, give up!", type.c_str());
			return IResourcePtr();
		}

		// load resource through implicit loading function
		res = loadResourceImpl(handle, name, group, resourceType, fileName, params, loader);

		// create an instance
		if (res == NULL){
			res = loader->createResourceInstance(resourceType, params);
			if (res == NULL){
				NR_Log(Log::LOG_ENGINE, Log::LL_ERROR, "ResourceManager: Resource instance could not be created. Abort.");
				return IResourcePtr();
			}
		}

		// check now if a empty resource of that type already exists and load it if not
		SharedPtr<IResource> empty;
		checkEmptyResource(resourceType, empty, loader);

		// create new handle
		handle = mLastHandle++;
		NR_Log(Log::LOG_ENGINE, "ResourceManager: Load resource %s (%s) from %s", name.c_str(), group.c_str(), fileName.c_str());

		// load the resource
		res->mResLoader = loader;
		res->mResFileName = fileName;
		res->mResGroup = group;
		res->mResName = name;
		res->mResHandle = handle;
		res->mParentManager = this;
		Result ret = loader->loadResource(res);
		if (ret != OK){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Could not load resource");
			return IResourcePtr();
		}

		// create a holder for that resource
		SharedPtr<ResourceHolder> holder(new ResourceHolder());
		holder->resetRes(res);
		holder->setEmptyResource(empty);

		// store the resource in database
		mResourceGroup[group].push_back(handle);
		mResource[handle] = holder;
		mResourceName[name] = handle;

		// check for memory usage
		mMemUsage += holder->getResource()->getResDataSize();
		ret = checkMemoryUsage();
		if (ret != OK){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Could not rearrange memory");
			return IResourcePtr();
		}

		// return a pointer to that resource
		return IResourcePtr(holder);
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::checkEmptyResource(const ::std::string resourceType,
													SharedPtr<IResource>& empty,
													ResourceLoader loader)
	{

		// check now if a empty resource of that type already exists and load it if not
		res_empty_map::iterator it = mEmptyResource.find(resourceType);
		if (it == mEmptyResource.end()){
			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG,
				"ResourceManager: Create empty resource of type %s", resourceType.c_str());

			empty.reset(loader->createEmptyResource(resourceType));
			if (empty.get() == NULL){
				NR_Log(Log::LOG_ENGINE, "ResourceManager: Cannot load empty resource of type %s", resourceType.c_str());
				return RES_CAN_NOT_LOAD_EMPTY;
			}
			empty->mResGroup = "__Empty_Resource_Group__";
			empty->mResName = "__Empty_" + resourceType;
			empty->mResHandle = 0;
			empty->mParentManager = this;
			mEmptyResource[resourceType] = empty;

			return OK;
		}

		// so we did find such one, so return it back
		empty = it->second;
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unload(const ::std::string& name){

		ResourcePtr<IResource> res = getByName(name);

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockPure(res);
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Unload resource %s",
				res->getResName().c_str(), res->getResHandle());
			Result ret = res->unloadRes();
		unlockPure(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unload(IResourcePtr& res){

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockPure(res);
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Unload resource %s",
				res->getResName().c_str(), res->getResHandle());
			Result ret = res->unloadRes();
		unlockPure(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unload(ResourceHandle& handle){

		ResourcePtr<IResource> res = getByHandle(handle);

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockPure(res);
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Unload resource %s",
				res->getResName().c_str(), res->getResHandle());
			Result ret = res->unloadRes();
		unlockPure(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::reload(const ::std::string& name){

		ResourcePtr<IResource> res = getByName(name);

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockPure(res);
			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceManager: Reload resource %s",
				res->getResName().c_str(), res->getResHandle());
			Result ret = res->reloadRes();
		unlockPure(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::reload(ResourceHandle& handle){

		ResourcePtr<IResource> res = getByHandle(handle);

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockPure(res);
			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceManager: Reload resource %s",
				res->getResName().c_str(), res->getResHandle());
			Result ret = res->reloadRes();
		unlockPure(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::reload(IResourcePtr& res){

		if (res.isNull()){
			return RES_NOT_FOUND;
		}

		lockPure(res);
			NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "ResourceManager: Reload resource %s",
				res->getResName().c_str(), res->getResHandle());
			Result ret = res->reloadRes();
		unlockPure(res);

		return ret;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::removeImpl(IResourcePtr& resPtr){

		NR_ASSERT(!resPtr.isNull() && "Given pointer shouldn't be zero");

		// we need to lock the resource, because we need the real resource object not empty
		lockPure(resPtr);

			ResourcePtr<IResource> ptr = resPtr;
			ResourceHandle hdl = ptr->getResHandle();
			::std::string name = ptr->getResName();
			::std::string grp = ptr->getResGroup();

			if (hdl == 0){
				NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not remove resource %s because it is not in the database", name.c_str());
				return RES_NOT_FOUND;
			}

			// first unload it
			unload(resPtr);

			NR_Log(Log::LOG_ENGINE, "ResourceManager: Remove resource %s (%s)", name.c_str(), grp.c_str());

			// remove the handle from the group list
			if (grp.length() > 0){
				res_grp_map::iterator jt = mResourceGroup.find(grp);
				if (jt != mResourceGroup.end()){
					jt->second.remove(hdl);

					// check whenever group contains nomore elements, and delete it
					if (jt->second.size() == 0){
						mResourceGroup.erase(grp);
						NR_Log(Log::LOG_ENGINE, "ResourceManager: %s's group \"%s\" does not contain elements, so remove it", name.c_str(), grp.c_str());
					}else{
						NR_Log(Log::LOG_ENGINE, "ResourceManager: There still some resources in the %s's group \"%s\"", name.c_str(), grp.c_str());
					}
				}else{
					NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "ResourceManager: There is no group in the database according to resource's group name \"%s\"", grp.c_str());
				}
			}

		// unlock it back
		unlockPure(resPtr);

		// clear the database
		mResourceName.erase(name);
		mResource.erase(hdl);

		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::remove(const ::std::string& name){

		// check whenever such a resource exists
		IResourcePtr ptr = getByName(name);

		if (ptr.isNull()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Remove resource %s not found", name.c_str());
			return RES_NOT_FOUND;
		}

		return removeImpl(ptr);
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::remove(ResourceHandle& handle){

		// check whenever such a resource exists
		IResourcePtr ptr = getByHandle(handle);

		if (ptr.isNull()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Remove resource %d not found", handle);
			return RES_NOT_FOUND;
		}

		return removeImpl(ptr);
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::remove(IResourcePtr& ptr){

		// check whenever such a resource exists
		if (!ptr.isNull()){
			return removeImpl(ptr);
		}
		return OK;
	}

	//----------------------------------------------------------------------------------
	SharedPtr<ResourceHolder>* ResourceManager::getHolderByName(const ::std::string& name)
	{
		// find the handle
		res_str_map::iterator it = mResourceName.find(name);
		if (it == mResourceName.end()){
			return NULL;
		}

		// get through the handle the holder
		res_hdl_map::iterator jt = mResource.find(it->second);
		NR_ASSERT(jt != mResource.end() && "Fatal Error in the Database !!!");
		return &(jt->second);
	}

	//----------------------------------------------------------------------------------
	SharedPtr<ResourceHolder>* ResourceManager::getHolderByHandle(const ResourceHandle& handle)
	{
		// find through the handle
		res_hdl_map::iterator it = mResource.find(handle);
		if (it == mResource.end()){
			return NULL;
		}
		return &(it->second);
	}

	//----------------------------------------------------------------------------------
	IResourcePtr ResourceManager::getByName(const ::std::string& name){
		SharedPtr<ResourceHolder>* holder = getHolderByName(name);
		if (holder == NULL){
			return IResourcePtr();
		}

		return IResourcePtr(*holder);
	}

	//----------------------------------------------------------------------------------
	IResourcePtr ResourceManager::getByHandle(const ResourceHandle& handle){
		SharedPtr<ResourceHolder>* holder = getHolderByHandle(handle);
		if (holder == NULL){
			return IResourcePtr();
		}
		return IResourcePtr(*holder);
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::checkMemoryUsage(){
		// TODO: Has to be implemented
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::add(IResource* res, const ::std::string& name, const ::std::string& group){

		// check for bad parameters
		if (res == NULL) return OK;
		if (name.length() == 0) return BAD_PARAMETERS;
		if (res->mResLoader == NULL) return RES_LOADER_NOT_EXISTS;


		// check whenever such resource already exists
		if (!getByName(name).isNull()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: WARNING: Add resource %s but it already exists", name.c_str());
			return OK;
		}

		ResourceHandle handle = mLastHandle++;
		NR_Log(Log::LOG_ENGINE, "ResourceManager: Add resource %s id=%d to the database from outside", name.c_str(), handle);

		// check now if a empty resource of that type already exists and load it if not
		SharedPtr<IResource> empty;
		checkEmptyResource(res->getResType(), empty, res->mResLoader);

		// create a holder for that resource
		SharedPtr<ResourceHolder> holder(new ResourceHolder());
		holder->resetRes(res);
		holder->setEmptyResource(empty);

		// store the resource in database
		mResourceGroup[group].push_back(handle);
		mResource[handle] = holder;
		mResourceName[name] = handle;

		res->mResGroup = group;
		res->mResName = name;
		res->mResHandle = handle;
		res->mParentManager = this;

		// check for memory usage
		mMemUsage += holder->getResource()->getResDataSize();
		Result ret = checkMemoryUsage();

		if (ret != OK){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Could not rearrange memory");
			return ret;
		}

		// return a pointer to that resource
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::lockPure(const ::std::string& name){

		// get appropriate pointer
		IResourcePtr ptr = getByName(name);

		// if it is null, so we do not have such a resource
		if (ptr.isNull()) return RES_NOT_FOUND;

		return ptr.lockPure();
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::lockPure(ResourceHandle& handle){

		// get appropriate pointer
		IResourcePtr ptr = getByHandle(handle);

		// if it is null, so we do not have such a resource
		if (ptr.isNull()) return RES_NOT_FOUND;

		return ptr.lockPure();

	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::lockPure(IResourcePtr& res){

		// lock through the pointer
		return res.lockPure();

	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unlockPure(const ::std::string& name){

		// get appropriate holder
		SharedPtr<ResourceHolder>* holder = getHolderByName(name);

		if (holder != NULL){
			// lock the resource through the holder
			(*holder)->unlockPure();
		}else{
			return RES_NOT_FOUND;
		}

		return OK;

	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unlockPure(ResourceHandle& handle){

		// get appropriate holder
		SharedPtr<ResourceHolder>* holder = getHolderByHandle(handle);

		if (holder != NULL){
			// lock the resource through the holder
			(*holder)->unlockPure();
		}else{
			return RES_NOT_FOUND;
		}

		return OK;

	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unlockPure(IResourcePtr& res){

		// if pointer does not pointing anywhere
		if (res.isNull()){
			return RES_ERROR;
		}

		// lock through the holder
		res.getResourceHolder()->unlockPure();

		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::unloadGroup(const ::std::string& group){

		// check whenever such a group exists
		res_grp_map::const_iterator it = mResourceGroup.find(group);
		if (it == mResourceGroup.end()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not unload group \"%s\" because not found in database", group.c_str());
			return RES_GROUP_NOT_FOUND;
		}

		NR_Log(Log::LOG_ENGINE, "ResourceManager: Unload the group \"%s\"", group.c_str());

		// scan through all elements
		::std::list<ResourceHandle>::iterator jt = mResourceGroup[group].begin();
		for (; jt != mResourceGroup[group].end(); jt++){
			Result ret = unload(*jt);
			if (ret != OK) return ret;
		}

		// OK
		return OK;
	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::reloadGroup(const ::std::string& group){

		// check whenever such a group exists
		res_grp_map::const_iterator it = mResourceGroup.find(group);
		if (it == mResourceGroup.end()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not reload group \"%s\" because not found in database", group.c_str());
			return RES_GROUP_NOT_FOUND;
		}

		NR_Log(Log::LOG_ENGINE, "ResourceManager: Reload the group \"%s\"", group.c_str());

		// scan through all elements
		::std::list<ResourceHandle>::iterator jt = mResourceGroup[group].begin();
		for (; jt != mResourceGroup[group].end(); jt++){
			Result ret = reload(*jt);
			if (ret != OK) return ret;
		}

		// OK
		return OK;

	}

	//----------------------------------------------------------------------------------
	Result ResourceManager::removeGroup(const ::std::string& group){

		// check whenever such a group exists
		res_grp_map::const_iterator it = mResourceGroup.find(group);
		if (it == mResourceGroup.end()){
			NR_Log(Log::LOG_ENGINE, "ResourceManager: Can not remove group \"%s\" because not found in database", group.c_str());
			return RES_GROUP_NOT_FOUND;
		}

		NR_Log(Log::LOG_ENGINE, "ResourceManager: Remove all elements from the group \"%s\"", group.c_str());

		// scan through all elements
		::std::list<ResourceHandle>::iterator jt = mResourceGroup[group].begin();
		for (; jt != mResourceGroup[group].end(); jt++){
			Result ret = remove(*jt);
			if (ret != OK) return ret;
		}

		// remove the group
		mResourceGroup.erase(group);

		// OK
		return OK;

	}

	//----------------------------------------------------------------------------------
	void ResourceManager::_notifyResourceLoaded(ResourceHandle& handle){

	}

	//----------------------------------------------------------------------------------
	void ResourceManager::_notifyResourceUnloaded(ResourceHandle& handle){

	}

};

