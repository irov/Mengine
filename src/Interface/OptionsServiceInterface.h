#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ArgumentsInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OptionsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "OptionsService" )

    public:
        virtual bool setArguments( const ArgumentsInterfacePtr & _arguments ) = 0;

    public:
        virtual bool hasOption( const Char * _key ) const = 0;
        virtual const Char * getOptionValue( const Char * _key, const Char * _default ) const = 0;
        virtual uint32_t getOptionUInt32( const Char * _key ) const = 0;
        virtual bool testOptionValue( const Char * _key, const Char * _value ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define OPTIONS_SERVICE()\
    ((Mengine::OptionsServiceInterface *)SERVICE_GET(Mengine::OptionsServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define HAS_OPTION( Key )\
    ([](){static bool value = OPTIONS_SERVICE()->hasOption( Key ); return value;}())
//////////////////////////////////////////////////////////////////////////
#define GET_OPTION_VALUE( Key, Default )\
    ([](){static const Mengine::Char * value = OPTIONS_SERVICE()->getOptionValue( Key, Default ); return value;}())
//////////////////////////////////////////////////////////////////////////
#define GET_OPTION_VALUE_UINT32( Key )\
    ([](){static uint32_t value = OPTIONS_SERVICE()->getOptionUInt32( Key ); return value;}())
//////////////////////////////////////////////////////////////////////////
#define TEST_OPTION_VALUE( Key, Value )\
    OPTIONS_SERVICE()->testOptionValue( Key, Value )
//////////////////////////////////////////////////////////////////////////

