#pragma once

#include "Interface/ConfigInterface.h"
#include "Interface/InputStreamInterface.h"

#include "Kernel/Tags.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MemoryConfig
        : public ConfigInterface
    {
        DECLARE_FACTORABLE( MemoryConfig );

    public:
        MemoryConfig();
        ~MemoryConfig() override;

    public:
        struct RecordDesc
        {
            String section;
            String key;
            String value;
        };

        typedef Vector<RecordDesc> VectorRecords;

    public:
        void setPlatformTags( const Tags & _platformTags ) override;
        const Tags & getPlatformTags() const override;

    public:
        bool load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) override;

    public:
        bool existValue( const Char * _section, const Char * _key ) const override;

    public:
        bool hasValue( const Char * _section, const Char * _key, bool * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, int8_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, uint8_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, int32_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, uint32_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, int64_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, uint64_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, float * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, double * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const Char ** const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, ConstString * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, FilePath * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, Tags * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, Resolution * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, Color * const _value ) const override;

    public:
        bool getValue( const Char * _section, const Char * _key, bool _default ) const override;
        int8_t getValue( const Char * _section, const Char * _key, int8_t _default ) const override;
        uint8_t getValue( const Char * _section, const Char * _key, uint8_t _default ) const override;
        int32_t getValue( const Char * _section, const Char * _key, int32_t _default ) const override;
        uint32_t getValue( const Char * _section, const Char * _key, uint32_t _default ) const override;
        int64_t getValue( const Char * _section, const Char * _key, int64_t _default ) const override;
        uint64_t getValue( const Char * _section, const Char * _key, uint64_t _default ) const override;
        float getValue( const Char * _section, const Char * _key, float _default ) const override;
        double getValue( const Char * _section, const Char * _key, double _default ) const override;
        const Char * getValue( const Char * _section, const Char * _key, const Char * _default ) const override;
        ConstString getValue( const Char * _section, const Char * _key, const ConstString & _default ) const override;
        FilePath getValue( const Char * _section, const Char * _key, const FilePath & _default ) const override;
        Tags getValue( const Char * _section, const Char * _key, const Tags & _default ) const override;
        Resolution getValue( const Char * _section, const Char * _key, const Resolution & _default ) const override;
        Color getValue( const Char * _section, const Char * _key, const Color & _default ) const override;

    public:
        void getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorString * const _values ) const override;

    public:
        void setValue( const Char * _section, const Char * _key, const Char * _value ) override;

    public:
        bool hasSection( const Char * _section ) const override;

    protected:
        Tags m_platformTags;

        VectorRecords m_records;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MemoryConfig, ConfigInterface> MemoryConfigPtr;
    //////////////////////////////////////////////////////////////////////////
}


