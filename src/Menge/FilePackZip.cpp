/*
 *	FilePackZip.cpp
 *
 *	Created by _Berserk_ on 16.3.2009
 *	Copyright 2009 Menge. All rights reserved.
 *
 */

#	include "FilePackZip.h"
#	include "Interface/FileSystemInterface.h"

#	define ZIP_LOCAL_FILE_HEADER_SIGNATURE	0x04034b50
#	define MAX_FILENAME 1024

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FilePackZip::FilePackZip( DataStreamInterface* _stream )
		: FilePack( _stream )
	{
		parsePack_();
	}
	//////////////////////////////////////////////////////////////////////////
	FilePackZip::~FilePackZip()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DataStreamInterface* FilePackZip::openFile( const String& _filename )
	{
		TFileInfoMap::iterator it_find = m_files.find( _filename );
		if( it_find == m_files.end() )
		{
			return NULL;
		}

		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FilePackZip::hasFile( const String& _filename )
	{
		TFileInfoMap::iterator it_find = m_files.find( _filename );
		return ( it_find != m_files.end() );
	}
	//////////////////////////////////////////////////////////////////////////
	void FilePackZip::parsePack_()
	{
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
		char fileName[MAX_FILENAME];
		while( m_stream->eof() == false )
		{
			m_stream->read( &signature, sizeof( signature ) );
			if( signature != ZIP_LOCAL_FILE_HEADER_SIGNATURE )
			{
				break;
			}
			m_stream->read( &versionNeeded, sizeof( versionNeeded ) );
			m_stream->read( &generalPurposeFlag, sizeof( generalPurposeFlag ) );
			m_stream->read( &compressionMethod, sizeof( compressionMethod ) );
			m_stream->read( &lastModTime, sizeof( lastModTime ) );
			m_stream->read( &lastModDate, sizeof( lastModDate ) );
			m_stream->read( &crc32, sizeof( crc32 ) );
			m_stream->read( &compressedSize, sizeof( compressedSize ) );
			m_stream->read( &uncompressedSize, sizeof( uncompressedSize ) );
			m_stream->read( &fileNameLen, sizeof( fileNameLen ) );
			m_stream->read( &extraFieldLen, sizeof( extraFieldLen ) );
			m_stream->read( &fileName, fileNameLen );
			m_stream->skip( extraFieldLen );
			String filename( fileName, fileNameLen );
			if( compressedSize > 0 )	// if not folder
			{
				FileInfo fi = { m_stream->tell(), compressedSize };
				m_files.insert( std::make_pair( filename, fi ) );
				++m_fileCount;
			}
			m_stream->skip( compressedSize );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
