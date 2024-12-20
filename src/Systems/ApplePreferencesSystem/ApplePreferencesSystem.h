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
        bool setPreferenceInteger( const Char * _key, int64_t _value ) override;
        int64_t getPreferenceInteger( const Char * _key, int64_t _default ) const override;
        
        bool setPreferenceString( const Char * _key, const Char * _value ) override;
        bool getPreferenceString( const Char * _key, Char * const _value, size_t _capacity, size_t * const _size ) const override;
        
        bool setPreferenceConstString( const Char * _key, const ConstString & _value ) override;
        ConstString getPreferenceConstString( const Char * _key, const ConstString & _default ) const override;        

    public:
        bool hasPreference( const Char * _key ) const override;
        void removePreference( const Char * _key ) override;
    };
};
