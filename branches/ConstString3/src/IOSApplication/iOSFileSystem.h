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
	
class iOSFileInput : public FileInputInterface
{
	FILE *							stream;
public:
									iOSFileInput( void );
	
	virtual bool					open( const String& _filename );
	virtual void					close( void );
	
	virtual bool					time( time_t & _time ) const;
	
	virtual int						read( void * _buf, int _count );
	virtual void					seek( int _pos );
	virtual int						tell( void ) const;
	virtual int						size( void ) const;
};
	
class iOSFileOutput : public FileOutputInterface
{
	FILE *							stream;
public:
									iOSFileOutput( void );
	
	virtual bool					open( const String& _filename );
	virtual void					close( void );
	virtual int						tell( void );
	
	virtual void					write( const void* _data, int _count );
	virtual void					flush( void );
};

class iOSFileSystem : public FileSystemInterface
{
	/*char							resDirectory[ 1024 ];
	char							docDirectory[ 1024 ];*/
	
	std::map< void *, FILE * >		mappedFiles;
public:
									iOSFileSystem( void );
	virtual							~iOSFileSystem( void );
	
	FILE * const					Open( const char * name, const char * mode );
	
	/*const char * const				GetResDir( void ) const;
	const char * const				GetDocDir( void ) const;*/
	
	virtual bool					existFile( const String& _filename );
	
	virtual InputStreamInterface *	openInputStream( const String& _filename );
	virtual void					closeInputStream( InputStreamInterface* _stream );
	
	virtual OutputStreamInterface *	openOutputStream( const String& _filename );
	virtual void					closeOutputStream( OutputStreamInterface* _stream );
	
	virtual void *					openMappedFile( const String& _filename, int* _size );
	virtual void					closeMappedFile( void* _file );
	
	virtual bool					deleteFile( const String& _filename );
	virtual bool					createFolder( const String& _path );
	virtual bool					deleteFolder( const String& _path );
};
	
}

bool initInterfaceSystem( Menge::FileSystemInterface * * _ptrFileSystem );
void releaseInterfaceSystem( Menge::FileSystemInterface * _ptrFileSystem );
