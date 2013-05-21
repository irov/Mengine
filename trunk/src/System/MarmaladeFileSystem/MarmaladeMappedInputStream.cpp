#	include "MarmaladeMappedInputStream.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/UnicodeInterface.h"

#   include "Core/MemoryProxyInput.h"

#   include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	MarmaladeMappedInputStream::MarmaladeMappedInputStream()
        : m_serviceProvider(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MarmaladeMappedInputStream::~MarmaladeMappedInputStream()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr MarmaladeMappedInputStream::createInputMemory()
    {
        MemoryInput * memory = m_factoryMemoryInput.createObjectT();

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeMappedInputStream::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeMappedInputStream::openInputMemory( const InputStreamInterfacePtr & _stream, const FilePath & _filename, size_t _offset, size_t _size )
    {
        MemoryInput * memory = intrusive_get<MemoryInput>(_stream);

        void * buffer = memory->newMemory( _size );

        m_inputStream.seek( _offset );
        m_inputStream.read( buffer, _size );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeMappedInputStream::open( const FilePath& _filePath )
    {
        if( m_inputStream.open( _filePath ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeMappedInputStream::_destroy()
    {
        if( m_inputStream._destroy() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MarmaladeMappedInputStream::read( void* _buf, size_t _count )
    {     
        size_t read_size = m_inputStream.read( _buf, _count );

        return read_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeMappedInputStream::seek( size_t _pos )
    {
        m_inputStream.seek( _pos );
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MarmaladeMappedInputStream::tell() const
    {
        size_t current = m_inputStream.tell();

        return current;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MarmaladeMappedInputStream::size() const 
    {
        size_t size = m_inputStream.size();

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeMappedInputStream::time( time_t & _time ) const
    {
        if( m_inputStream.time( _time ) == false )
        {
            return false;
        }

        return true;
    }
}	// namespace Menge
