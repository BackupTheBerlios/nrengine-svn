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


#ifndef _NR_FILESYSTEM_H_
#define _NR_FILESYSTEM_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "IFileSystem.h"

/**
* @defgroup vfs Engine's filesystem
*
* Files are storing the data needed for apropriate working of the engine and the
* underlying application. Files could be found on the disk but also on external media
* like ftp servers or similar things. You can either implement your own file
* loading routines to load such external files or you use the engine's file system
* which has the ability to be fully transparent to the user.
*
* Our filesystem contains a manger which does manage all available virtual file systems.
* Each such virtual file system (vfs) can be either a local fs working on local files
* or an http implementation of this one working on files in the internet. Each such
* vfs is registerd by the manager, so it knows how to handle file requests.
*
* Files are requested through it's name. So just ask a manger to open a file with name A.ext
* and it will ask each vfs if it does know such a file and ask them to open it. So for you
* as user there is no difference if the file is found on the local disk or on a ftp server.
* However transfering file data from external fs can be slow.
*
* If you know where the file is, you could also specify the file position directly, by
* specifying the file location similar to "file:/A.ext" for local files and "ftp:/A.ext"
* for external files. So the manager will ask that vfs thats type is specified for
* appropriate file. This mechanism is very similar to unix one, so you should be familar with it.
*
* There can also be some parameters to the certain vfs specified in the file path. For
* example you want to open a file on a certain ftp server and not the default one.
* So calling something like this "ftp:server.domain:user:password:/A.ext" will force the
* ftp-vfs to login on that server and download the file.
*
* We definy that all parameters given to the certain vfs systems should be separated by :
* Which parameters can be given you can find in the documentation of each fs module.
* There is no matter how much parameters you set, the filename must be at last.
* i.e. "filesystem:param1:param2:..:paramn:/path/to/file/file.name"
* The given filename is always in context to the filesystem. For example for local
* files you specify relative path, for ftp-servers it is always absolute.
* Filename-conventions are always according to the module you use. Our standard module
* to load local files, does use simple convention that all file names are relative to the
* application and the path is separated by slash. Conversion to windows filenames is
* done by the modules automaticaly, so "file:d:/A.ext" refers to a file on disk d in windows.
**/

namespace nrEngine{

 //! File system manager class handles all filesystem transparent to the user
 /**
  * Engine's does use a file system manger to manage all available file systems.
  * This fs is called virtual file system or VFS. You as user of the engine does
  * use this vfs to get the data from files. The system is working transparent, so
  * you does not notice where the files are readed from.
  *
  * Each certain module should register by this manager so the user get access to the
  * file sstem provided by the module.
  *
  * \ingroup vfs
  **/
 class _NRExport FileSystemManager : public ISingleton<FileSystemManager> {
		public:

			//! Initilize the virtual file system so it can be now accessed
			FileSystemManager();

			//! Release used memory and force all modules to unload
			~FileSystemManager();

			/**
			 * Register a new file system by the manager.
			 * The file systems are used to access the files
			 * 
			 * @param fs Smart pointer on the file system object
			 **/
			Result addFilesystem(SharedPtr<IFileSystem> fs);

			
			/**
			 * Remove a certain filesystem from the list. This will unload
			 * the filesystem smart pointer, so if it not used anymore the filesystem
			 * will be removed from the memory. This should also close all opened files.
			 *
			 * @param name Name of the file system
			 **/
			Result removeFilesystem(const std::string& name);

			
			/**
			 * Get the file system by it's type name.
			 *
			 * @param name Name of the file system
			 **/
			SharedPtr<IFileSystem> getFilesystem(const std::string& name);

			
			/**
			 **/


		private:
			//! Map of registered file systems with their type names
			typedef std::map< std::string, SharedPtr<IFileSystem> > FilesystemMap;

			FilesystemMap mFilesystems;

			
			
	};
								 
};

#endif
