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


#ifndef _NR_FILE_STREAM_H_
#define _NR_FILE_STREAM_H_


/*!
 * \defgroup filesys File system
 *
 * In our engine you have a defualt filesystem which you can you
 * for your own puprose. The filesystem we are using is based on std::ifstream
 * so it is fully compatible to all system where std::ifstream works.
 * The filesystem is built in the way that it can easy be replaced through
 * plugins which can expand the functionality of the system.
 *
 * Each file in the system is also a resource which is managed by resource
 * manager. We do not want to separate the filesystem from resoure system
 * (as in many engine's does) because so we get the ability of simple user interface
 * and the possibility of controlling the files as resources (e.g. if they are
 * used in streams and can be un/loaded at runtime).
 *
 * If files are handled as resources you can also still able to manage your
 * memory without forgetting of counting of the size which each file has.
 * For example unstreamed file which is loaded completely in the memory does
 * use a lot of free memory. The resource according to the file for example an image
 * is working on that file. In a naive way you will get only the size of the image
 * counted and the file size wuold be not mentioned in the resource system. So
 * you didn't realy realize how much memory does your resource need. With the system
 * we are using you know that the complete size of the resource is the image object
 * and the file opened by it.
 *
 * Also the possibility of using files like resource does centralize the managment
 * of all opperations according to loading/unloading/removing. So you can also use
 * scripts and just load resource "File" which is handled like normal resource but
 * provides reading to the file.
 *
 * Our filesystem has also the possibility to add an archive support to the filesystem.
 * This could be done by appropriate plugin.
 *
 * Actually our current implementation of the engine allows us to add the filesystem
 * functionality to the engine by plugins. But because we must be able to read simple
 * files like config file which is loaded before all plugins, we must find the way
 * how to do this. So using of the filesystem as plugin is not possible.
 * But you are still able to add more supported file types (filesystems) through
 * plugins after this system is running.
 *
 * @see ResourceManager, IResource, IStream
 **/

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "IStream.h"
#include "Resource.h"

namespace nrEngine{

	//! Derived class from IStream to provide streaming from files
	/**
	* FileStream is derived class from IStream and IResource. It provides
	* the functionality of streaming the files from the disk. We use the
	* std::ifstream class to do this work.
	*
	* This class is a simple file class which provide the user only
	* with nessary methods. You should use plugins to extend the functionality
	* for working with compressed, encrypted etc. file archives.
	*
	* Because each file is also a resource, so you can open the files through
	* the resource manager. This will call the appropriate resource loader
	* (in our case file loader) to provide you with the file you requested.
	* The type of such a resource is "File"
	* Plugins can add new types like "ZipFile", "ZipArchive", ...
	*
	* \ingroup filesys
	**/
	class _NRExport FileStream : public IStream, public IResource {
	public:

		/**
		* Constructor of the file stream. The loader should
		* specify all neccessary properties of this object to allow
		* opening of the file.
		**/
		FileStream();

		/**
		* Virtual destructor allows to derive new classes from this interface
		**/
		virtual ~FileStream();


		/**
		 * Open a file as a stream without using of underlying resource system.
		 **/
		virtual Result open (const ::std::string& fileName);

		//------------------------------------------------------------
		//		IStream Interface
		//------------------------------------------------------------

		/**
		* Return the name of the stream. The name of the stream is the same
		* as the name of the underlying resource used for the file storing.
		**/
		const ::std::string& getName()	const { return getResName(); }

		/**
		* @copydoc IStream::read()
		**/
		virtual size_t read(void *buf, size_t size, size_t nmemb);

		/**
		* @copydoc IStream::readDelim()
		**/
		virtual size_t readDelim(void* buf, size_t count, const ::std::string& delim = ::std::string("\n"));

		/**
		* @copydoc IStream::tell()
		**/
		virtual size_t tell() const;

		/**
		* @copydoc IStream::eof()
		**/
		virtual bool eof() const;

		/**
		* @copydoc IStream::getData()
		**/
		virtual byte* getData(size_t& count) const;

		/**
		* @copydoc IStream::seek()
		**/
		virtual bool seek(int32 offset, int32 whence = IStream::CURRENT);

		/**
		*  @copydoc IStream::close()
		**/
		virtual void close ();


		//------------------------------------------------------------
		//		IResource Interface
		//------------------------------------------------------------

		/**
		* The file stream is unloaded if the underlying stream is closed
		* so no operations are possible.
		* @copydoc IResource::unloadRes()
		**/
		virtual Result unloadRes();

		/**
		* Just reopen the file stream and start from the beginning.
		* @copydoc IResource::reloadRes()
		**/
		virtual Result reloadRes();


	private:

		//! The file stream loader should be able to change the data of this object
		friend class FileStreamLoader;

		//! Stream used to read the file
		SharedPtr< ::std::ifstream > mStream;

	};


	//! Empty file stream to represent empty files
	/**
	 * This is  file stream which loads an empty file. You will
	 * need this to get ResourceManagment working corectly. Any
	 * access to a resource instantiated from this class have
	 * no effect.
	 **/
	class _NRExport EmptyFileStream : public FileStream {

	public:
		//! Initialize the empty file
		EmptyFileStream();

		//! Deinitilize the empty file object
		~EmptyFileStream();

		//! Read data from the empty file returns nothing
		size_t read(void *buf, size_t size, size_t nmemb);

		//! Read until we fund a delimeter
		size_t readDelim(void* buf, size_t count, const ::std::string& delim = "\n");

		//! What is the position of the empty file reading cursor
		size_t tell() const;

		//! Are we on the end of file. For empty files it is always true
		bool eof() const;

		//! Returns no data from the empty file
		byte* getData(size_t& count) const;

		//! Seek or not to seek? That is the question!
		bool seek(int32 offset, int32 whence = IStream::CURRENT);

		//! Close the empty file
		void close ();


		//! Unload the empty file resource
		Result unloadRes();

		//! Reloade the empty file resource
		Result reloadRes();

	};


}; // end namespace

#endif
