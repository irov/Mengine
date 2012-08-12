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

#   include "metabuf/ArchiveReader.hpp"

#   include <stdio.h>

#	define ZIP_LOCAL_FILE_HEADER_SIGNATURE	0x04034b50
#	define MAX_FILENAME 1024

namespace Menge
{
	struct ZipHeader
	{
		uint16 versionNeeded;
		uint16 generalPurposeFlag;
		uint16 compressionMethod;
		uint16 lastModTime;
		uint16 lastModDate;
		uint32 crc32;
		uint32 compressedSize;
		uint32 uncompressedSize;
		//uint64 compressedSize64;
		//uint64 uncompressedSize64;
		uint16 fileNameLen;
		uint16 extraFieldLen;
	};
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
        
		UnicodeServiceInterface * unicodeService = ServiceProvider::get()
			->getServiceT<UnicodeServiceInterface>("UnicodeService");

		char fileNameBuffer[MAX_FILENAME];

		while( Utils::eof( m_zipFile ) == false )
		{
			m_zipFile->read( &signature, sizeof( signature ) );

			if( signature != ZIP_LOCAL_FILE_HEADER_SIGNATURE )
			{
				break;
			}
            			
            ZipHeader header;

            //const size_t headerSize = sizeof(header.versionNeeded)
            //    +sizeof(header.generalPurposeFlag)
            //    +sizeof(header.compressionMethod)
            //    +sizeof(header.lastModTime)
            //    +sizeof(header.lastModDate)
            //    +sizeof(header.crc32)
            //    +sizeof(header.compressedSize)
            //    +sizeof(header.uncompressedSize)
            //    +sizeof(header.fileNameLen)
            //    +sizeof(header.extraFieldLen);

            //char headerData[headerSize];

            //m_zipFile->read( headerData, headerSize );

            //size_t size_read;
            //Metabuf::ArchiveReader ar( headerData, headerSize, size_read );

            //ar >> header.versionNeeded;
            //ar >> header.generalPurposeFlag;
            //ar >> header.compressionMethod;
            //ar >> header.lastModTime;
            //ar >> header.lastModDate;
            //ar >> header.crc32;
            //ar >> header.compressedSize;
            //ar >> header.uncompressedSize;
            //ar >> header.fileNameLen;
            //ar >> header.extraFieldLen;

            
            m_zipFile->read( &header.versionNeeded, sizeof( header.versionNeeded ) );
            m_zipFile->read( &header.generalPurposeFlag, sizeof( header.generalPurposeFlag ) );
            m_zipFile->read( &header.compressionMethod, sizeof( header.compressionMethod ) );
            m_zipFile->read( &header.lastModTime, sizeof( header.lastModTime ) );
            m_zipFile->read( &header.lastModDate, sizeof( header.lastModDate ) );
            m_zipFile->read( &header.crc32, sizeof( header.crc32 ) );
            m_zipFile->read( &header.compressedSize, sizeof( header.compressedSize ) );
            m_zipFile->read( &header.uncompressedSize, sizeof( header.uncompressedSize ) );
            m_zipFile->read( &header.fileNameLen, sizeof( header.fileNameLen ) );
            m_zipFile->read( &header.extraFieldLen, sizeof( header.extraFieldLen ) );
			
			m_zipFile->read( &fileNameBuffer, header.fileNameLen );
			
			Utils::skip( m_zipFile, header.extraFieldLen );
			String fileNameA( fileNameBuffer, header.fileNameLen );

			//if( compressedSize == 0 ) // if folder
			//{
			//	continue;
			//}
						
			if( header.compressionMethod != 0 )
			{
				MENGE_LOG_ERROR( "Warning: (FileSystemZip::initialize) compressed file '%s'"
					, fileNameA.c_str() 
					);

				continue;
			}

			FileInfo fi = { m_zipFile->tell(), header.compressedSize, header.uncompressedSize, header.compressionMethod };

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
			
			Utils::skip( m_zipFile, header.compressedSize );
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
		MemoryFileInput* memFile = static_cast<MemoryFileInput*>( _file );
		//m_fileInputPool.release( memFile );
		delete memFile;
	}
	//////////////////////////////////////////////////////////////////////////
	FileBufferProvider * FileSystemZip::getBufferProvider() const
	{
		//TODO
		return 0;
	}
}	// namespace Menge
