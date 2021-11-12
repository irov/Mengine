#pragma once

#include "Interface/OptionsServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class OptionsService
        : public ServiceBase<OptionsServiceInterface>
    {
    public:
        OptionsService();
        ~OptionsService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool setArguments( const ArgumentsInterfacePtr & _arguments ) override;

    public:
        bool hasOption( const Char * _key ) const override;
        bool setOptionValue( const Char * _key, const Char * _value ) override;
        const Char * getOptionValue( const Char * _key, const Char * _default ) const override;
        bool getOptionValues( const Char * _key, const Char ** _values, uint32_t * _count ) const override;
        uint32_t getOptionUInt32( const Char * _key, uint32_t _default ) const override;
        bool testOptionValue( const Char * _key, const Char * _value ) const override;

    protected:
        bool logOptions_();

    protected:
        struct Option
        {
            Char key[MENGINE_OPTIONS_KEY_SIZE] = {'\0'};
            Char value[MENGINE_OPTIONS_VALUE_COUNT][MENGINE_OPTIONS_VALUE_SIZE] = {'\0'};
            uint32_t value_count;
        };

        typedef Vector<Option> VectorOptions;
        VectorOptions m_options;
    };
}


