/*
 *  iOSFileSystem.cpp
 *  Mengine
 *
 *  Created by Dgut on 9/18/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include "iOSFileSystem.h"
#include "Menge/FileEngine.h"
#include <sys/mman.h>
#include <sys/stat.h>

#include "CoreFoundation/CoreFoundation.h"
#import <UIKit/UIKit.h>

class Utf8
{
public:
    Utf8(const wchar_t* wsz): m_utf8(NULL)
    {
        // OS X uses 32-bit wchar
        const int bytes = wcslen(wsz) * sizeof(wchar_t);

        CFStringRef str = CFStringCreateWithBytesNoCopy(NULL, 
                                                        (const UInt8*)wsz, bytes, 
                                                        kCFStringEncodingUTF32LE, false, 
                                                        kCFAllocatorNull
                                                        );
        
        const int bytesUtf8 = CFStringGetMaximumSizeOfFileSystemRepresentation(str);
        m_utf8 = new char[bytesUtf8];
        CFStringGetFileSystemRepresentation(str, m_utf8, bytesUtf8);
        CFRelease(str);
    }   
    
    ~Utf8() 
    { 
        if( m_utf8 )
        {
            delete[] m_utf8;
        }
    }
    
public:
    const char* c_str() const { return m_utf8; }
    
private:
    char* m_utf8;
};

bool initInterfaceSystem( Menge::FileSystemInterface ** _ptrFileSystem )
{
	try
	{
		*_ptrFileSystem = new Menge::iOSFileSystem();
	}
	
	catch( ... )
	{
		return false;
	}
	
	return true;
}

void releaseInterfaceSystem( Menge::FileSystemInterface * _ptrFileSystem )
{
	delete static_cast< Menge::iOSFileSystem * >( _ptrFileSystem );
}

namespace Menge
{

iOSFileInput::iOSFileInput( void ) :
	stream	( 0 )
{
}
	
bool iOSFileInput::open( const WString & _filename )
{
    const Utf8 utf8 = _filename.c_str();
    
	stream = fopen( utf8.c_str(), "rb" );

	return stream;
}

void iOSFileInput::close( void )
{
	fclose( stream );
}
	
bool iOSFileInput::time( time_t & _time ) const
{
	return false;
}
	
int iOSFileInput::read( void * _buf, int _count )
{
	return fread( _buf, _count, 1, stream ) * _count;
}
	
void iOSFileInput::seek( int _pos )
{
	fseek( stream, _pos, SEEK_SET );
}
	
int iOSFileInput::tell( void ) const
{
	return ftell( stream );
}
	
int iOSFileInput::size( void ) const
{
	int cur = ftell( stream );
	fseek( stream, 0, SEEK_END );
	int size = ftell( stream );
	fseek( stream, cur, SEEK_SET );
	return size;
}
	
iOSFileOutput::iOSFileOutput( void ) :
	stream	( 0 )
{
}

bool iOSFileOutput::open( const WString & _filename )
{
    const Utf8 utf8 = _filename.c_str();
    
	stream = fopen( utf8.c_str(), "wb" );
    
	return stream;
}
	
void iOSFileOutput::close( void )
{
	fclose( stream );
}
	
int iOSFileOutput::tell( void )
{
	return ftell( stream );
}
	
void iOSFileOutput::write( const void * _data, int _count )
{
	fwrite( _data, _count, 1, stream );
}
	
void iOSFileOutput::flush( void )
{
	fflush( stream );
}
	
iOSFileSystem::iOSFileSystem( void )
{
	/*[ [ [ NSBundle mainBundle] resourcePath ] getCString : resDirectory
											   maxLength : sizeof( resDirectory ) - 1
												encoding : NSASCIIStringEncoding ];
	
	[ [ NSSearchPathForDirectoriesInDomains( NSDocumentDirectory, NSUserDomainMask, YES ) objectAtIndex : 0 ]
																							 getCString : docDirectory
																							 maxLength : sizeof( docDirectory ) - 1
																							 encoding : NSASCIIStringEncoding ];*/
}
	
iOSFileSystem::~iOSFileSystem( void )
{
}

/*const char * const iOSFileSystem::GetResDir( void ) const
{
	return resDirectory;
}

const char * const iOSFileSystem::GetDocDir( void ) const
{
	return docDirectory;
}*/

bool iOSFileSystem::existFile( const WString & _filename ) const
{  
	/*char temp[ 1024 ];
	
	sprintf( temp, "%s/%s", docDirectory, _filename.c_str() );
	if( access( temp, F_OK ) != -1 )
		return true;
	
	sprintf( temp, "%s/%s", resDirectory, _filename.c_str() );
	return access( temp, F_OK ) != -1;*/
    
    const Utf8 utf8 = _filename.c_str();
   
	int result = access( utf8.c_str(), F_OK );

    return result != -1;
}

FileInputStreamInterface * iOSFileSystem::openInputStream( const WString & _filename )
{
	iOSFileInput * f = new iOSFileInput();
        
	if( !f->open( _filename ) )
    {
        delete f;
        
		return 0;
    }
    
	return f;
}

void iOSFileSystem::closeInputStream( FileInputStreamInterface* _stream )
{
	if( !_stream )
		return;
    
    iOSFileInput * f = static_cast<iOSFileInput *>( _stream );
    
	f->close();
	delete f;
}

FileOutputStreamInterface * iOSFileSystem::openOutputStream( const WString & _filename )
{
	iOSFileOutput * f = new iOSFileOutput();
	f->open( _filename );
    
	return f;
}

void iOSFileSystem::closeOutputStream( FileOutputStreamInterface * _stream )
{
    iOSFileOutput * f = static_cast<iOSFileOutput *>( _stream );
	f->close();
    
	delete f;
}

void * iOSFileSystem::openMappedFile( const WString & _filename, int * _size )
{
	if( !existFile( _filename ) )
		return 0;
	
    const Utf8 utf8 = _filename.c_str();
	FILE * stream = fopen( utf8.c_str(), "w+b" );
	if( !stream )
		return 0;
	
	fseek( stream, 0, SEEK_END );
	*_size = ftell( stream );
	fseek( stream, 0, SEEK_SET );
	
	void * ptr = mmap( 0, *_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno( stream ), 0 );
	
	mappedFiles[ ptr ] = stream;
	
	return ptr;
}

void iOSFileSystem::closeMappedFile( void * _file )
{
	if( !mappedFiles[ _file ] )
		return;
	
	FILE * stream = mappedFiles[ _file ];
	mappedFiles[ _file ] = 0;
	
	fseek( stream, 0, SEEK_END );
	int size = ftell( stream );
	fseek( stream, 0, SEEK_SET );
	
	munmap( _file, size );
	
	fclose( stream );
}

bool iOSFileSystem::deleteFile( const WString & _filename )
{
	/*char temp[ 1024 ];
	sprintf( temp, "%s/%s", docDirectory, _filename.c_str() );
	return remove( temp );*/
    const Utf8 utf8 = _filename.c_str();
	return !remove( utf8.c_str() );
}

bool iOSFileSystem::createFolder( const WString & _path )
{
	/*char temp[ 1024 ];
	sprintf( temp, "%s/%s", docDirectory, _path.c_str() );
	return !mkdir( temp, 0777 );*/
    
    const Utf8 utf8 = _path.c_str();
	return !mkdir( utf8.c_str(), 0777 );
}

bool iOSFileSystem::deleteFolder( const WString & _path )
{
	/*char temp[ 1024 ];
	sprintf( temp, "%s/%s", docDirectory, _path.c_str() );
	return !rmdir( temp );*/
    
    const Utf8 utf8 = _path.c_str();
	return !rmdir( utf8.c_str() );
}

}
