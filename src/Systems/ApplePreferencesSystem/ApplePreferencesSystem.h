#pragma once

#include "Interface/PreferencesSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class ApplePreferencesSystem
        : public ServiceBase<PreferencesSystemInterface>
    {
    public:
        ApplePreferencesSystem();
        ~ApplePreferencesSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        int64_t getPreferenceInteger( const Char * _key, int64_t _default ) const override;
        bool setPreferenceInteger( const Char * _key, int64_t _value ) override;
        ConstString getPreferenceString( const Char * _key, const ConstString & _default ) const override;
        bool setPreferenceString( const Char * _key, const ConstString & _value ) override;
    };
};
