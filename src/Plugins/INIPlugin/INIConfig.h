#pragma once

#include "Interface/ConfigInterface.h"
#include "Interface/InputStreamInterface.h"

#include "INIUtils.h"

#include "Kernel/Tags.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<INIUtils::IniStore> VectorIniStores;
    //////////////////////////////////////////////////////////////////////////
    class INIConfig
        : public ConfigInterface
    {
        DECLARE_FACTORABLE( INIConfig );

    public:
        INIConfig();
        ~INIConfig() override;

    public:
        void setPlatformTags( const Tags & _platformTags ) override;
        const Tags & getPlatformTags() const override;

    public:
        bool load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) override;

    public:
        bool existValue( const Char * _section, const Char * _key ) const override;

    public:
        bool hasValue( const Char * _section, const Char * _key, bool _default, bool * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, int8_t _default, int8_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, uint8_t _default, uint8_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, int32_t _default, int32_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, uint32_t _default, uint32_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, int64_t _default, int64_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, uint64_t _default, uint64_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, float _default, float * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, double _default, double * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const Char * _default, const Char ** const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const ConstString & _default, ConstString * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const FilePath & _default, FilePath * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const Tags & _default, Tags * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const Resolution & _default, Resolution * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const Color & _default, Color * const _value ) const override;

    public:
        bool hasSection( const Char * _section ) const override;

    public:
        void getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorString * const _values ) const override;

    protected:
        Tags m_platformTags;

        VectorIniStores m_stores;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<INIConfig, ConfigInterface> INIConfigPtr;
    //////////////////////////////////////////////////////////////////////////
}


