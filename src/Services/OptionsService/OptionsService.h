#pragma once

#include "Interface/OptionsInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Char.h"

namespace Mengine
{
#ifndef MENGINE_OPTIONS_KEY_SIZE
#	define MENGINE_OPTIONS_KEY_SIZE 32
#endif

#ifndef MENGINE_OPTIONS_VALUE_SIZE
#	define MENGINE_OPTIONS_VALUE_SIZE 32
#endif

    class OptionsService
        : public ServiceBase<OptionsServiceInterface>
    {
    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setArgs( const VectorString & _args ) override;

    public:
        bool hasOption( const Char * _key ) const override;
        const Char * getOptionValue( const Char * _key ) const override;
        bool testOptionValue( const Char * _key, const Char * _value ) const override;

    protected:
        struct Option
        {
            Char key[MENGINE_OPTIONS_KEY_SIZE];
            Char value[MENGINE_OPTIONS_VALUE_SIZE];
        };

        typedef Vector<Option> VectorOptions;
        VectorOptions m_options;
    };
}


