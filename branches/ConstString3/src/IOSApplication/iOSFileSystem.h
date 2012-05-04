/*
 *  iOSFileSystem.h
 *  Mengine
 *
 *  Created by Dgut on 9/18/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "Interface/FileSystemInterface.h"
#include <map>

namespace Menge
{
	
class iOSFileInput : public FileInputStreamInterface
{
	FILE * stream;
    
public:
	iOSFileInput( void );
	
	bool open( const WString& _filename );
	void close( void );
	
	bool time( time_t & _time ) const;
	
	int read( void * _buf, int _count );
	void seek( int _pos );
	int	tell( void ) const;
    int size( void ) const;
};
	
class iOSFileOutput : public FileOutputStreamInterface
{
	FILE * stream;
    
public:
    iOSFileOutput( void );
	
	bool open( const WString& _filename );
	void close( void );
	int	tell( void );
	
	void write( const void* _data, int _count );
	void flush( void );
};

class iOSFileSystem : public FileSystemInterface
{
	/*char							resDirectory[ 1024 ];
	char							docDirectory[ 1024 ];*/
	
	std::map< void *, FILE * > mappedFiles;
    
public:
	iOSFileSystem( void );
	~iOSFileSystem( void );

	/*const char * const				GetResDir( void ) const;
	const char * const				GetDocDir( void ) const;*/
	
	bool existFile( const WString& _filename ) const override;
	
	FileInputStreamInterface *	openInputStream( const WString& _filename ) override;
	void closeInputStream( FileInputStreamInterface* _stream ) override;
	
	FileOutputStreamInterface *	openOutputStream( const WString& _filename ) override;
	void closeOutputStream( FileOutputStreamInterface* _stream ) override;
	
	void * openMappedFile( const WString& _filename, int* _size ) override;
	void closeMappedFile( void* _file ) override;
	
	bool deleteFile( const WString& _filename ) override;
	bool createFolder( const WString& _path ) override;
	bool deleteFolder( const WString& _path ) override;
};
	
}

bool initInterfaceSystem( Menge::FileSystemInterface * * _ptrFileSystem );
void releaseInterfaceSystem( Menge::FileSystemInterface * _ptrFileSystem );
