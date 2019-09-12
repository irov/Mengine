#pragma once

#include "Interface/ConfigInterface.h"
#include "Interface/InputStreamInterface.h"

#include "IniUtil.h"

#include "Kernel/Factorable.h"
#include "Kernel/Tags.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<IniUtil::IniStore> VectorIniStores;
    //////////////////////////////////////////////////////////////////////////
    class IniConfig
        : public ConfigInterface
        , public Factorable
    {
    public:
        IniConfig();
        ~IniConfig() override;

    public:
        void setPlatformTags( const Tags & _platformTags ) override;
        const Tags & getPlatformTags() const override;

    public:
        bool load( const InputStreamInterfacePtr & _stream );

    public:
        bool existValue( const Char * _section, const Char * _key ) const override;

    public:
        bool hasValue( const Char * _section, const Char * _key, bool * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, int8_t * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, uint8_t * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, int32_t * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, uint32_t * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, uint64_t * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, float * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, double * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const Char ** _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, ConstString * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, FilePath * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, Tags * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, Resolution * _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, Color * _value ) const override;

    public:
        bool hasSection( const Char * _section ) const override;

    public:
        bool getValue( const Char * _section, const Char * _key, bool _default ) const override;
        int8_t getValue( const Char * _section, const Char * _key, int8_t _default ) const override;
        uint8_t getValue( const Char * _section, const Char * _key, uint8_t _default ) const override;
        int32_t getValue( const Char * _section, const Char * _key, int32_t _default ) const override;
        uint32_t getValue( const Char * _section, const Char * _key, uint32_t _default ) const override;
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
        void getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports & _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorFilePath & _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorConstString & _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorString & _values ) const override;

    public:
        void setValue( const Char * _section, const Char * _key, const Char * _value ) override;

    protected:
        Tags m_platformTags;

        VectorIniStores m_stores;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<IniConfig, ConfigInterface> INIConfigPtr;
    //////////////////////////////////////////////////////////////////////////
}


