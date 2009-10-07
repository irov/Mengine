/*
 *	PosixFileSystem.cpp
 *
 *	Created by _Berserk_ on 7.10.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "PosixFileSystem.h"

#	include <unistd.h>
#	include <sys/mman.h>

#	include "PosixInputStream.h"
#	include "PosixOutputStream.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PosixFileSystem::PosixFileSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PosixFileSystem::~PosixFileSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileSystem::existFile( const String& _filename )
	{
		return access( _filename.c_str(), F_OK ) == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	InputStreamInterface* PosixFileSystem::openInputStream( const String& _filename )
	{
		PosixInputStream* inputStream = NULL;

		if( m_inputStreamPool.empty() == true )
		{
			inputStream = new PosixInputStream();
		}
		else
		{
			inputStream = m_inputStreamPool.back();
			m_inputStreamPool.pop_back();
		}

		if( inputStream->open( _filename ) == false )
		{				
			this->closeInputStream( inputStream );
			inputStream = NULL;
		}

		return inputStream;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixFileSystem::closeInputStream( InputStreamInterface* _stream )
	{
		PosixInputStream* inputStream = static_cast<PosixInputStream*>( _stream );
		if( inputStream != NULL )
		{
			inputStream->close();
			m_inputStreamPool.push_back( inputStream );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	OutputStreamInterface* PosixFileSystem::openOutputStream( const String& _filename )
	{
		PosixOutputStream* outStream = new PosixOutputStream();
		if( outStream->open( _filename ) == false )
		{
			delete outStream;
			outStream = NULL;
		}
		return outStream;

	}
	//////////////////////////////////////////////////////////////////////////
	void PosixFileSystem::closeOutputStream( OutputStreamInterface* _stream )
	{
		delete static_cast<PosixOutputStream*>( _stream );
	}
	//////////////////////////////////////////////////////////////////////////
	void* PosixFileSystem::openMappedFile( const String& _filename, int* _size )
	{
		int hFile = ::open( _filename.c_str(), O_RDONLY );
		if( hFile == -1 )
		{
			return NULL;
		}

		stat hStat;
		std::fill( static_cast<unsigned char*>( &hStat ),
			static_cast<unsigned char*>( &hStat ) + sizeof( stat ), 0 );
		if( fstat( hFile, &hStat ) != 0 )
		{
			return NULL;
		}

		void* pMem = ::mmap( NULL, hStat.st_size, PROT_NONE, MAP_PRIVATE, hFile, 0 );
		if( pMem == MAP_FAILED )
		{
			return NULL;
		}

		FileMappingInfo fmInfo = { hFile, hStat.st_size };
		m_fileMappingMap.insert( std::make_pair( pMem, fmInfo ) );

		return pMem;
	}
	//////////////////////////////////////////////////////////////////////////
	void PosixFileSystem::closeMappedFile( void* _file )
	{
		TFileMappingMap::iterator it_find = m_fileMappingMap.find( _file );
		if( it_find != m_fileMappingMap.end() )
		{
			::munmap( _file, it_find->second.size );
			::close( it_find->second.hFile );
			m_fileMappingMap.erase( it_find );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileSystem::deleteFile( const String& _filename )
	{
		return ::remove( _filename.c_str() ) == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileSystem::createFolder( const String& _path )
	{
		return ::mkdir( _path.c_str() ) == 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PosixFileSystem::deleteFolder( const String& _path )
	{
		DIR *pdir = NULL;
		pdir = opendir (path.c_str());
		if (pdir == NULL)	// if pdir wasn't initialised correctly
		{					
			return false;	// return false to say "we couldn't do it"
		}
		struct dirent *pent = NULL;

		int counter = 1;				// use this to skip the first TWO which cause an infinite loop (and eventually, stack overflow)
		while (pent = readdir (pdir))	// while there is still something in the directory to list
		{
			if (counter > 2) 
			{
				if (pent == NULL)		// if pent has not been initialised correctly
				{
					return false; // we couldn't do it
				} 

				// otherwise, it was initialised correctly, so let's delete the file~
				String filename = _path;
				filename += "/";
				filename += String( pent->d_name );
				//if (IsDirectory(filename) == true) 
				//{
				//	deleteFolder(filename);
				//} 
				//else // it's a file, we can use remove
				//{
				//	remove( filename.c_str() );
				//}
				if( deleteFile( filename ) == false )	// oops maybe that was a directory
				{
					if( deleteFolder( filename ) == false )		// some shit happened
					{
						return false;
					}
				}
			} 
			counter++;
		}

		// finally, let's clean up
		closedir( pdir ); // close the directory
		if ( rmdir( _path.c_str() ) == false ) 
		{
			return false; // delete the directory
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
