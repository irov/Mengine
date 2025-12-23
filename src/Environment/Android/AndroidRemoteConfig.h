#pragma once

#include "Interface/ConfigInterface.h"

namespace Mengine
{
    class AndroidRemoteConfig
        : public ConfigInterface
    {
        DECLARE_FACTORABLE( INIConfig );

    public:
        AndroidRemoteConfig();
        ~AndroidRemoteConfig() override;

    public:
        void setPlatformTags( const Tags & _platformTags ) override;
        const Tags & getPlatformTags() const override;

    public:
        bool load( const InputStreamInterfacePtr & _stream, const DocumentInterfacePtr & _doc ) override;
        void unload() override;

    public:
        bool existValue( const Char * _section, const Char * _key ) const override;

    public:
        bool hasValue( const Char * _section, const Char * _key, bool _default, bool * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, int64_t _default, int64_t * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, double _default, double * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const String & _default, String * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const ConstString & _default, ConstString * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const FilePath & _default, FilePath * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const Tags & _default, Tags * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const Resolution & _default, Resolution * const _value ) const override;
        bool hasValue( const Char * _section, const Char * _key, const Color & _default, Color * const _value ) const override;

    public:
        bool hasSection( const Char * _section ) const override;

    public:
        bool setValue( const Char * _section, const Char * _key, bool _value ) override;
        bool setValue( const Char * _section, const Char * _key, int64_t _value ) override;
        bool setValue( const Char * _section, const Char * _key, double _value ) override;
        bool setValue( const Char * _section, const Char * _key, const String & _value ) override;
        bool setValue( const Char * _section, const Char * _key, const ConstString & _value ) override;
        bool setValue( const Char * _section, const Char * _key, const FilePath & _value ) override;
        bool setValue( const Char * _section, const Char * _key, const Tags & _value ) override;
        bool setValue( const Char * _section, const Char * _key, const Resolution & _value ) override;
        bool setValue( const Char * _section, const Char * _key, const Color & _value ) override;

    public:
        void getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const override;
        void getValues( const Char * _section, const Char * _key, VectorString * const _values ) const override;

    protected:
        Tags m_platformTags;
    };
}
