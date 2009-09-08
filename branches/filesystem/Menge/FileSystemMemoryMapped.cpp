/*
 *	FileSystemMemoryMapped.cpp
 *
 *	Created by _Berserk_ on 8.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "FileSystemMemoryMapped.h"

#	include <cassert>

#	include "FileEngine.h"
#	include "MemoryFileInput.h"
#	include "Utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileSystemMemoryMapped::FileSystemMemoryMapped()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	FileSystemMemoryMapped::~FileSystemMemoryMapped()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemMemoryMapped::initialize( const String& _path, bool _create )
	{
		m_path = _path;
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemMemoryMapped::existFile( const String& _filename )
	{
		String fullname;
		makeFullname_( _filename, &fullname );

		TMappedFilesMap::iterator it_find = m_files.find( fullname );
		return it_find != m_files.end();
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputInterface* FileSystemMemoryMapped::createInputFile()
	{
		MemoryFileInput* memFile = m_fileInputPool.get();
		memFile->setFileSystem( this );
		return memFile;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemMemoryMapped::openInputFile( const String& _filename, FileInputInterface* _file )
	{
		String fullname;
		makeFullname_( _filename, &fullname );

		assert( _file != NULL );
		MemoryFileInput* memFile = static_cast< MemoryFileInput* >( _file );

		TMappedFilesMap::iterator it_find = m_files.find( fullname );
		if( it_find != m_files.end() )
		{
			it_find->second.refCount += 1;
			memFile->setMemory( it_find->second.pMem, it_find->second.size );
		}
		else
		{
			FileSystemInterface* fsInterface = Holder<FileEngine>::hostage()
													->getFileSystemInterface();

			int size = 0;
			void* pMem = fsInterface->openMappedFile( fullname, &size );
			if( pMem == NULL )
			{
				return false;
			}

			memFile->setMemory( pMem, size );
			FileInfo fi = { pMem, size, 1 };
			m_files.insert( std::make_pair( fullname, fi ) );
		}
		m_memFileMap.insert( std::make_pair( memFile, fullname ) );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemMemoryMapped::closeInputFile( FileInputInterface* _file )
	{
		MemoryFileInput* memFile = static_cast< MemoryFileInput* >( _file );
		TMemFileMap::iterator it_find_memfile = m_memFileMap.find( memFile );
		if( it_find_memfile != m_memFileMap.end() )
		{
			TMappedFilesMap::iterator it_find = m_files.find( it_find_memfile->second );
			if( it_find != m_files.end() )
			{
				it_find->second.refCount -= 1;
				if( it_find->second.refCount == 0 )
				{
					FileSystemInterface* fsInterface = Holder<FileEngine>::hostage()
						->getFileSystemInterface();
					fsInterface->closeMappedFile( it_find->second.pMem );
					m_files.erase( it_find );
				}
			}
			m_memFileMap.erase( it_find_memfile );
		}
		m_fileInputPool.release( memFile );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemMemoryMapped::makeFullname_( const String& _path, String* _fullname )
	{
		assert( _fullname != NULL );
		if( m_path.empty() == false )
		{
			*_fullname = m_path;
			(*_fullname) += "/";
			(*_fullname) += _path;
		}
		else
		{
			(*_fullname) = _path;
		}
		Utils::collapsePath( (*_fullname), _fullname );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
