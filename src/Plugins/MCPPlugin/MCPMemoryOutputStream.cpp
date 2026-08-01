#include "MCPMemoryOutputStream.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPMemoryOutputStream::MCPMemoryOutputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPMemoryOutputStream::~MCPMemoryOutputStream()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MCPMemoryOutputStream::write( const void * _data, size_t _size )
    {
        if( _size == 0 )
        {
            return 0;
        }

        size_t offset = m_data.size();
        m_data.resize( offset + _size );
        uint8_t * data = m_data.data();
        uint8_t * destination = data + offset;
        StdString::memcpy( destination, _data, _size );

        return _size;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MCPMemoryOutputStream::size() const
    {
        size_t size = m_data.size();

        return size;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPMemoryOutputStream::flush()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const Data & MCPMemoryOutputStream::getData() const
    {
        const Data & data = m_data;

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
}
