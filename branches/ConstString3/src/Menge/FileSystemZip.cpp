/*
 *	FileSystemZip.cpp
 *
 *	Created by _Berserk_ on 7.9.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "FileSystemZip.h"

#	include "FileEngine.h"
#	include "LogEngine.h"
#	include "MemoryFileInput.h"
#	include "Core/File.h"
#	include "Core/String.h"
#	include "ServiceProvider.h"
#	include "Interface/UnicodeInterface.h"
#	define ZIP_LOCAL_FILE_HEADER_SIGNATURE	0x04034b50
#	define MAX_FILENAME 1024

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FileSystemZip::FileSystemZip()
		: m_fileEngine(NULL)
		, m_zipFile(NULL)
	{

	}
	//////////////////////////////////////////////////////////////////////////
	FileSystemZip::~FileSystemZip()
	{
		if ( m_interface == NULL )
		{
			return;
		}

		if( m_zipFile == NULL )
		{
			return;
		}

		m_interface->closeMappedInputStream( m_zipFile );
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemZip::initialize( const WString& _path, FileEngine * _fileEngine, bool _create )
	{
		m_fileEngine = _fileEngine;

		m_interface = m_fileEngine->getFileSystemInterface();

		if( m_interface == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileSystemZip::initialize) can't get FileSystem interface" );

			return false;
		}

		m_path = _path;

		m_zipFile = m_interface->openMappedInputStream( m_path );

		if( m_zipFile == NULL )
		{
			MENGE_LOG_ERROR( "Error: (FileSystemZip::initialize) can't open zip file data" );
			
			return false;
		}
		
		uint32 signature = 0;
		uint16 versionNeeded = 0;
		uint16 generalPurposeFlag = 0;
		uint16 compressionMethod = 0;
		uint16 lastModTime = 0;
		uint16 lastModDate = 0;
		uint32 crc32 = 0;
		uint32 compressedSize = 0;
		uint32 uncompressedSize = 0;
		uint64 compressedSize64 = 0;
		uint64 uncompressedSize64 = 0;
		uint16 fileNameLen = 0;
		uint16 extraFieldLen = 0;
		char fileNameBuffer[MAX_FILENAME];

		UnicodeServiceInterface * unicodeService = ServiceProvider::get()
			->getServiceT<UnicodeServiceInterface>("UnicodeService");

		while( Utils::eof( m_zipFile ) == false )
		{
			m_zipFile->read( &signature, sizeof( signature ) );

			if( signature != ZIP_LOCAL_FILE_HEADER_SIGNATURE )
			{
				break;
			}

			m_zipFile->read( &versionNeeded, sizeof( versionNeeded ) );
			m_zipFile->read( &generalPurposeFlag, sizeof( generalPurposeFlag ) );
			m_zipFile->read( &compressionMethod, sizeof( compressionMethod ) );
			m_zipFile->read( &lastModTime, sizeof( lastModTime ) );
			m_zipFile->read( &lastModDate, sizeof( lastModDate ) );
			m_zipFile->read( &crc32, sizeof( crc32 ) );
			m_zipFile->read( &compressedSize, sizeof( compressedSize ) );
			m_zipFile->read( &uncompressedSize, sizeof( uncompressedSize ) );
			m_zipFile->read( &fileNameLen, sizeof( fileNameLen ) );
			m_zipFile->read( &extraFieldLen, sizeof( extraFieldLen ) );
			m_zipFile->read( &fileNameBuffer, fileNameLen );

			Utils::skip( m_zipFile, extraFieldLen );
			String fileNameA( fileNameBuffer, fileNameLen );

			if( compressedSize > 0 )	// if not folder
			{
				if( compressionMethod != 0 )
				{
					MENGE_LOG_ERROR( "Warning: (FileSystemZip::initialize) compressed file '%s'"
						, fileNameA.c_str() 
						);

					continue;
				}

				FileInfo fi = { m_zipFile->tell(), compressedSize, uncompressedSize, compressionMethod };
				
				bool success;
				WString fileNameU = unicodeService->utf8ToUnicode( fileNameA, success );
				if( success == false )
				{
					MENGE_LOG_ERROR( "ERROR: (FileSystemZip::initialize) can`t create unicode name  '%s'"
						, fileNameA.c_str() 
						);

					continue;
				}

				m_files.insert( std::make_pair( fileNameU, fi ) );
			}

			Utils::skip( m_zipFile, compressedSize );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & FileSystemZip::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemZip::existFile( const WString& _filename )
	{
		TMapFileInfo::iterator it_find = m_files.find( _filename );
		if( it_find != m_files.end() )
		{
			return true;
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	FileInputStreamInterface* FileSystemZip::createInputFile()
	{
		//MemoryFileInput* memFile = m_fileInputPool.get();
		MemoryFileInput * memFile = new MemoryFileInput;
		memFile->setFileSystem( this );
		return memFile;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FileSystemZip::openInputFile( const WString& _filename, FileInputStreamInterface* _file )
	{
		if( _file == 0 )
		{
			return false;
		}

		TMapFileInfo::iterator it_find = m_files.find( _filename );
		if( it_find == m_files.end() )
		{
			return false;
		}

		MemoryFileInput* memFile = static_cast<MemoryFileInput*>(_file);

		unsigned char* pMem = static_cast<unsigned char*>(m_zipFile->getMemory());

		memFile->setMemory( pMem + it_find->second.seek_pos, it_find->second.file_size );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void FileSystemZip::closeInputFile( FileInputStreamInterface* _file )
	{
		//MemoryFileInput* memFile = static_cast<MemoryFileInput*>( _file );
		//m_fileInputPool.release( memFile );
		//delete memFile;
	}
	//////////////////////////////////////////////////////////////////////////
	FileBufferProvider * FileSystemZip::getBufferProvider() const
	{
		//TODO
		return 0;
	}
}	// namespace Menge
