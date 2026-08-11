#pragma once

#include "Option.h"

#include "Interface/FactoryInterface.h"
#include "Interface/OptionsServiceInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{
    class OptionsService
        : public ServiceBase<OptionsServiceInterface>
    {
        DECLARE_FACTORABLE( OptionsService );

    public:
        OptionsService();
        ~OptionsService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool setArguments( const ArgumentsInterfacePtr & _arguments ) override;

    public:
        bool hasOption( const Char * _key, bool _withValue ) const override;
        bool setOptionValue( const Char * _key, const Char * _value ) override;
        const Char * getOptionValue( const Char * _key, const Char * _default ) const override;
        bool getOptionValues( const Char * _key, const Char ** _values, uint32_t * const _count ) const override;
        int32_t getOptionInt32( const Char * _key, int32_t _default ) const override;
        uint32_t getOptionUInt32( const Char * _key, uint32_t _default ) const override;
        bool testOptionValue( const Char * _key, const Char * _value ) const override;

    public:
        void foreachOptions( const LambdaOption & _lambda ) const override;

    protected:
        FactoryInterfacePtr m_factoryOption;

        typedef Vector<OptionInterfacePtr> VectorOptions;
        VectorOptions m_options;
    };
}


