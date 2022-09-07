#pragma once

#include "Interface/ConfigInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "Kernel/Tags.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MultiConfig
        : public ConfigInterface
    {
        DECLARE_FACTORABLE( MultiConfig );

    public:
        MultiConfig();
        ~MultiConfig() override;

    public:
        void setMutex( const ThreadMutexInterfacePtr & _mutex );
        const ThreadMutexInterfacePtr & getMutex() const;

    public:
        void addConfig( const ConfigInterfacePtr & _config );

    public:
        typedef Lambda<void(const ConfigInterfacePtr &)> LambdaConfigs;
        void foreachConfig( const LambdaConfigs & _lambda );

    public:
        void setPlatformTags( const Tags & _platformTags ) override;
        const Tags & getPlatformTags() const override;

    public:
        bool load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) override;
        void unload() override;

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
        bool setValue( const Char * _section, const Char * _key, bool _value ) override;
        bool setValue( const Char * _section, const Char * _key, int8_t _value ) override;
        bool setValue( const Char * _section, const Char * _key, uint8_t _value ) override;
        bool setValue( const Char * _section, const Char * _key, int32_t _value ) override;
        bool setValue( const Char * _section, const Char * _key, uint32_t _value ) override;
        bool setValue( const Char * _section, const Char * _key, int64_t _value ) override;
        bool setValue( const Char * _section, const Char * _key, uint64_t _value ) override;
        bool setValue( const Char * _section, const Char * _key, float _value ) override;
        bool setValue( const Char * _section, const Char * _key, double _value ) override;
        bool setValue( const Char * _section, const Char * _key, const Char * _value ) override;
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

    public:
        bool hasSection( const Char * _section ) const override;

    protected:
        ThreadMutexInterfacePtr m_mutex;

        Tags m_platformTags;

        typedef Vector<ConfigInterfacePtr> VectorConfigs;
        VectorConfigs m_configs;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MultiConfig, ConfigInterface> MultiConfigPtr;
    //////////////////////////////////////////////////////////////////////////
}