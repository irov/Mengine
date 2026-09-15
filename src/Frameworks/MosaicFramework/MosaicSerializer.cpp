#include "MosaicSerializer.h"

#include "Config/StdIO.h"
#include "Config/StdLib.h"
#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MosaicSerializer::MosaicSerializer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicSerializer::~MosaicSerializer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicSerializer::begin( uint32_t _version )
    {
        m_buffer.clear();

        Char header[64] = {'\0'};
        MENGINE_SNPRINTF( header, 63, "version\t%u\n", _version );

        m_buffer.append( header );
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicSerializer::write( Mosaic::StringView _section, Mosaic::Id _id, Mosaic::StringView _key, Mosaic::StringView _value )
    {
        Char id[32] = {'\0'};
        MENGINE_SNPRINTF( id, 31, "%llu", (unsigned long long)_id );

        m_buffer.append( _section.data(), _section.size() );
        m_buffer.append( "\t" );
        m_buffer.append( id );
        m_buffer.append( "\t" );
        m_buffer.append( _key.data(), _key.size() );
        m_buffer.append( "\t" );
        m_buffer.append( _value.data(), _value.size() );
        m_buffer.append( "\n" );
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicSerializer::end()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    const String & MosaicSerializer::getBuffer() const
    {
        return m_buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicDeserializer::MosaicDeserializer()
        : m_version( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MosaicDeserializer::~MosaicDeserializer()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MosaicDeserializer::parse( const Char * _buffer, size_t _size )
    {
        m_records.clear();
        m_version = 0;

        String content( _buffer, _size );

        String::size_type offset = 0;

        while( offset < content.size() )
        {
            String::size_type lineEnd = content.find( '\n', offset );

            if( lineEnd == String::npos )
            {
                lineEnd = content.size();
            }

            String line = content.substr( offset, lineEnd - offset );

            offset = lineEnd + 1;

            if( line.empty() == true )
            {
                continue;
            }

            String::size_type first = line.find( '\t' );

            if( first == String::npos )
            {
                continue;
            }

            String head = line.substr( 0, first );

            if( head == "version" )
            {
                m_version = (uint32_t)StdLib::strtoull( line.c_str() + first + 1, nullptr, 10 );

                continue;
            }

            String::size_type second = line.find( '\t', first + 1 );

            if( second == String::npos )
            {
                continue;
            }

            String::size_type third = line.find( '\t', second + 1 );

            if( third == String::npos )
            {
                continue;
            }

            Record record;
            record.section = head;
            record.id = (Mosaic::Id)StdLib::strtoull( line.substr( first + 1, second - first - 1 ).c_str(), nullptr, 10 );
            record.key = line.substr( second + 1, third - second - 1 );
            record.value = line.substr( third + 1 );

            m_records.emplace_back( record );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MosaicDeserializer::version() const noexcept
    {
        return m_version;
    }
    //////////////////////////////////////////////////////////////////////////
    void MosaicDeserializer::forEach( void (*_visitor)(Mosaic::StringView, Mosaic::Id, Mosaic::StringView, Mosaic::StringView, void *), void * _data ) const
    {
        for( const Record & record : m_records )
        {
            (*_visitor)(Mosaic::StringView( record.section.c_str(), record.section.size() )
                , record.id
                , Mosaic::StringView( record.key.c_str(), record.key.size() )
                , Mosaic::StringView( record.value.c_str(), record.value.size() )
                , _data);
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
