/*
*  iOSFileSystem.cpp
*  Mengine
*
*  Created by Dgut on 9/18/11.
*  Copyright 2011 __MyCompanyName__. All rights reserved.
*
*/
#include "iOSFileSystem.h"
#include "Interface/FileSystemInterface.h"

#include "Core/MemoryProxyInput.h"

#include <sys/mman.h>
#include <sys/stat.h>

#include "CoreFoundation/CoreFoundation.h"

#import <UIKit/UIKit.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

extern "C" // only required if using g++
{
    bool createFileSystem( Menge::FileSystemInterface ** _fileSestem )
    {
        if( _fileSestem == NULL )
        {
            return false;
        }

        *_fileSestem = new Menge::iOSFileSystem;

        return true;
    }

    void destroyFileSystem( Menge::FileSystemInterface * _fileSestem )
    {
        if( _fileSestem )
        {
            delete static_cast<Menge::iOSFileSystem *>(_fileSestem);
        }
    }
}

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    iOSFileInput::iOSFileInput( ServiceProviderInterface * _serviceProvider ) 
        : m_serviceProvider(_serviceProvider)
        , stream(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSFileInput::open( const FilePath & _filename )
    {
        stream = fopen( _filename.c_str(), "rb" );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSFileInput::destroy()
    {
        if( stream == NULL )
        {
            return;
        }

        fclose( stream );

        delete this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSFileInput::time( time_t & _time ) const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    int iOSFileInput::read( void * _buf, int _count )
    {
        return fread( _buf, _count, 1, stream ) * _count;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSFileInput::seek( int _pos )
    {
        fseek( stream, _pos, SEEK_SET );
    }
    //////////////////////////////////////////////////////////////////////////
    int iOSFileInput::tell( void ) const
    {
        return ftell( stream );
    }
    //////////////////////////////////////////////////////////////////////////
    int iOSFileInput::size( void ) const
    {
        int cur = ftell( stream );

        fseek( stream, 0, SEEK_END );
        int size = ftell( stream );
        fseek( stream, cur, SEEK_SET );

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    iOSMappedFileInput::iOSMappedFileInput( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
        , m_id(-1)
        , m_memory(0)
        , m_data(0)
        , m_pos(NULL)
        , m_end(NULL)
        , m_size(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSMappedFileInput::open( const FilePath & _filename )
    {
        m_id = ::open( _filename.c_str(), O_RDONLY );

        if(m_id < 0)
        {
            return NULL;   
        }

        fcntl(m_id, F_NOCACHE, 1);
        fcntl(m_id, F_RDAHEAD, 1);

        struct stat statbuf;

        if( fstat(m_id, &statbuf) < 0 )
        {
            return NULL;
        }

        m_size = statbuf.st_size;

        m_memory = ::mmap(0, m_size, PROT_READ, MAP_SHARED, m_id, 0);

        m_data = static_cast<unsigned char*>( m_memory );

        m_pos = m_data;
        m_end = m_data + m_size;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSMappedFileInput::destroy()
    {
        if( m_id < 0 )
        {
            return;
        }
        
        ::munmap( m_memory, m_size );

        ::close( m_id );

        delete this;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSMappedFileInput::time( time_t & _time ) const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    int iOSMappedFileInput::read( void * _buf, int _count )
    {
        int cnt = _count;
        // Read over end of memory?
        if ( m_pos + cnt > m_end )
        {
            cnt = m_end - m_pos;
        }

        if ( cnt == 0 )
        {
            return 0;
        }

        std::copy( m_pos, m_pos + cnt, static_cast<unsigned char*>(_buf) );

        m_pos += cnt;

        return cnt;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSMappedFileInput::seek( int _pos )
    {
        if( _pos < 0 )
        {
            _pos = 0;
        }
        else if( _pos > m_size )
        {
            _pos = m_size;
        }

        m_pos = m_data + _pos;
    }
    //////////////////////////////////////////////////////////////////////////
    int iOSMappedFileInput::tell( void ) const
    {
        int distance = std::distance( m_data, m_pos );

        return distance;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterface * iOSMappedFileInput::createInputMemory()
    {
        MemoryProxyInput * memory = new MemoryProxyInput;

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSMappedFileInput::openInputMemory( InputStreamInterface * _stream, const FilePath & _filename, size_t _offset, size_t _size )
    {
        MemoryProxyInput * memory = static_cast<MemoryProxyInput *>(_stream);

        unsigned char* pMem = static_cast<unsigned char*>(m_memory);

        memory->setMemory( pMem + _offset, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    int iOSMappedFileInput::size( void ) const
    {
		return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    iOSFileOutput::iOSFileOutput( ServiceProviderInterface * _serviceProvider )
        : m_serviceProvider(_serviceProvider)
        , stream(0)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSFileOutput::open( const FilePath & _filename )
    {
        stream = fopen( _filename.c_str(), "wb" );

        if ( stream == 0 )
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSFileOutput::destroy( void )
    {
        fclose( stream );

        delete this;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSFileOutput::write( const void * _data, int _count )
    {
        fwrite( _data, _count, 1, stream );
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSFileOutput::flush( void )
    {
        fflush( stream );
    }
    //////////////////////////////////////////////////////////////////////////
    iOSFileSystem::iOSFileSystem( void )
    : m_serviceProvider(NULL)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSFileSystem::~iOSFileSystem( void )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSFileSystem::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * iOSFileSystem::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    MappedFileInputStreamInterface* iOSFileSystem::createMappedInputStream()
    {
        iOSMappedFileInput * f = new iOSMappedFileInput(m_serviceProvider);

        return f;
    }
    //////////////////////////////////////////////////////////////////////////
    FileInputStreamInterface* iOSFileSystem::createInputStream()
    {
        iOSFileInput * f = new iOSFileInput(m_serviceProvider);

        return f;
    }
    //////////////////////////////////////////////////////////////////////////
    FileOutputStreamInterface* iOSFileSystem::createOutputStream()
    {
        iOSFileOutput * f = new iOSFileOutput(m_serviceProvider);

        return f;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSFileSystem::existFile( const FilePath & _filename ) const
    {  
        int result = access( _filename.c_str(), F_OK );

        return result != -1;
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSFileSystem::deleteFile( const FilePath & _filename )
    {
        return !remove( _filename.c_str() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSFileSystem::createFolder( const FilePath & _path )
    {
        return !mkdir( _path.c_str(), 0777 );
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSFileSystem::deleteFolder( const FilePath & _path )
    {
        return !rmdir( _path.c_str() );
    }
    //////////////////////////////////////////////////////////////////////////
}
