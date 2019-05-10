#pragma once

#include "Interface/ConfigServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/Tags.h"
#include "Kernel/IniUtil.h"

namespace Mengine
{
    class ConfigService
        : public ServiceBase<ConfigServiceInterface>
    {
    public:
        ConfigService();
        ~ConfigService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool loadConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _publicConfigPath, const FilePath & _privateConfigPath ) override;

    public:
        bool getValue( const Char * _section, const Char * _key, bool _default ) const override;
        int32_t getValue( const Char * _section, const Char * _key, int32_t _default ) const override;
        uint32_t getValue( const Char * _section, const Char * _key, uint32_t _default ) const override;
        uint64_t getValue( const Char * _section, const Char * _key, uint64_t _default ) const override;
        float getValue( const Char * _section, const Char * _key, float _default ) const override;
        double getValue( const Char * _section, const Char * _key, double _default ) const override;
        const Char * getValue( const Char * _section, const Char * _key, const Char * _default ) const override;
        ConstString getValue( const Char * _section, const Char * _key, const ConstString & _default ) const override;
        FilePath getValue( const Char * _section, const Char * _key, const FilePath & _default ) const override;
        Resolution getValue( const Char * _section, const Char * _key, const Resolution & _default ) const override;

    public:
        void getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports & _value ) const override;
        void getValues( const Char * _section, const Char * _key, VectorString & _value ) const override;

    public:
        void getSection( const Char * _section, MapParams & _params ) const override;

    protected:
        Tags m_platformTags;

        IniUtil::IniStore m_publicINI;
        IniUtil::IniStore m_privateINI;
    };
}


