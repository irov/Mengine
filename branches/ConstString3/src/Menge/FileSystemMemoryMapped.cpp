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
#	include "Utils/Core/File.h"
#	include "Utils/Core/String.h"

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
	bool FileSystemMemoryMapped::initialize( const ConstString& _path, bool _create )
	{
		m_path = _path;
		Utils::collapsePath( m_path, m_path );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemMemoryMapped::existFile( const ConstString& _filename )
	{
		String fullname;
		makeFullname_( _filename, fullname );

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
	bool FileSystemMemoryMapped::openInputFile( const ConstString& _filename, FileInputInterface* _file )
	{
		String fullname;
		makeFullname_( _filename, fullname );

		assert( _file != NULL );
		MemoryFileInput* memFile = static_cast<MemoryFileInput*>( _file );

		TMappedFilesMap::iterator it_find = m_files.find( fullname );
		if( it_find != m_files.end() )
		{
			it_find->second.refCount += 1;
			memFile->setMemory( it_find->second.pMem, it_find->second.size );
		}
		else
		{
			FileSystemInterface* fsInterface = FileEngine::hostage()
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

		this->closeMemFile_( memFile );

		m_fileInputPool.release( memFile );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemMemoryMapped::closeMemFile_( MemoryFileInput* _file )
	{
		TMemFileMap::iterator it_find_memfile = m_memFileMap.find( _file );
		if( it_find_memfile == m_memFileMap.end() )
		{
			return;
		}
			
		TMappedFilesMap::iterator it_find = m_files.find( it_find_memfile->second );
		if( it_find == m_files.end() )
		{
			return;
		}
			
		it_find->second.refCount -= 1;
	
		if( it_find->second.refCount != 0 )
		{
			return;
		}
			
		FileSystemInterface* fsInterface = FileEngine::hostage()
											->getFileSystemInterface();
		
		fsInterface->closeMappedFile( it_find->second.pMem );
		m_files.erase( it_find );
		
		m_memFileMap.erase( it_find_memfile );
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemMemoryMapped::makeFullname_( const ConstString& _path, String & _fullname )
	{
		if( m_path.empty() == false )
		{
			_fullname = m_path;
			_fullname += "/";
			_fullname += _path;
		}
		else
		{
			_fullname = _path;
		}

		Utils::collapsePath( _fullname, _fullname );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
