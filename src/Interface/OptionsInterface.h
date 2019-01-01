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
        virtual void setArguments( const ArgumentsInterfacePtr & _arguments ) = 0;

    public:
        virtual bool hasOption( const Char * _key ) const = 0;
        virtual const Char * getOptionValue( const Char * _key ) const = 0;
        virtual bool testOptionValue( const Char * _key, const Char * _value ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define OPTIONS_SERVICE()\
    ((Mengine::OptionsServiceInterface *)SERVICE_GET(Mengine::OptionsServiceInterface))
//////////////////////////////////////////////////////////////////////////
#define HAS_OPTION( key )\
	([](){static bool value = OPTIONS_SERVICE()->hasOption( key ); return value;}())
//////////////////////////////////////////////////////////////////////////
#define GET_OPTION_VALUE( key )\
	([](){static const Char * value = OPTIONS_SERVICE()->getOptionValue( key ); return value;}())
//////////////////////////////////////////////////////////////////////////
#define TEST_OPTION_VALUE( key, value )\
	OPTIONS_SERVICE()->testOptionValue( key, value )
//////////////////////////////////////////////////////////////////////////
