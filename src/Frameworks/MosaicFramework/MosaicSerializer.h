#pragma once

#include "Kernel/String.h"
#include "Kernel/Vector.h"

#include "Mosaic/Persistence.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MosaicSerializer
        : public Mosaic::Serializer
    {
    public:
        MosaicSerializer();
        ~MosaicSerializer() override;

    public:
        void begin( uint32_t _version ) override;
        void write( Mosaic::StringView _section, Mosaic::Id _id, Mosaic::StringView _key, Mosaic::StringView _value ) override;
        void end() override;

    public:
        const String & getBuffer() const;

    protected:
        String m_buffer;
    };
    //////////////////////////////////////////////////////////////////////////
    class MosaicDeserializer
        : public Mosaic::Deserializer
    {
    public:
        MosaicDeserializer();
        ~MosaicDeserializer() override;

    public:
        bool parse( const Char * _buffer, size_t _size );

    public:
        uint32_t version() const noexcept override;
        void forEach( void (*_visitor)(Mosaic::StringView, Mosaic::Id, Mosaic::StringView, Mosaic::StringView, void *), void * _data ) const override;

    protected:
        struct Record
        {
            String section;
            Mosaic::Id id;
            String key;
            String value;
        };

        typedef Vector<Record> VectorRecords;
        VectorRecords m_records;

        uint32_t m_version;
    };
    //////////////////////////////////////////////////////////////////////////
}
