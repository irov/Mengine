#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ArgumentsInterface.h"

#include "Config/Char.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_OPTIONS_VALUES_MAX
#define MENGINE_OPTIONS_VALUES_MAX 16
#endif
//////////////////////////////////////////////////////////////////////////

namespace Mengine
{
    class OptionsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "OptionsService" )

    public:
        virtual bool setArguments( const ArgumentsInterfacePtr & _arguments ) = 0;

    public:
        virtual bool hasOption( const Char * _key, bool _withValue ) const = 0;
        virtual bool setOptionValue( const Char * _key, const Char * _value ) = 0;
        virtual const Char * getOptionValue( const Char * _key, const Char * _default ) const = 0;
        virtual bool getOptionValues( const Char * _key, const Char ** _values, uint32_t * const _count ) const = 0;
        virtual uint32_t getOptionUInt32( const Char * _key, uint32_t _default ) const = 0;
        virtual bool testOptionValue( const Char * _key, const Char * _value ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define OPTIONS_SERVICE()\
    ((Mengine::OptionsServiceInterface *)SERVICE_GET(Mengine::OptionsServiceInterface))
//////////////////////////////////////////////////////////////////////////