#pragma once

#include "Interface/ServiceInterface.h"

#include "Config/Typedef.h"
#include "Config/String.h"
#include "Config/VectorString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OptionsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "OptionsService" )

    public:
        virtual void setArgs( const VectorString & _args ) = 0;

    public:
        virtual bool hasOption( const Char * _key ) const = 0;
        virtual const Char * getOptionValue( const Char * _key ) const = 0;
        virtual bool testOptionValue( const Char * _key, const Char * _value ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
#define OPTIONS_SERVICE()\
    ((Mengine::OptionsServiceInterface *)SERVICE_GET(Mengine::OptionsServiceInterface))
    //////////////////////////////////////////////////////////////////////////
#define HAS_OPTION( key )\
	OPTIONS_SERVICE()->hasOption( key )
    //////////////////////////////////////////////////////////////////////////
#define GET_OPTION_VALUE( key )\
	OPTIONS_SERVICE()->getOptionValue( key )
    //////////////////////////////////////////////////////////////////////////
#define TEST_OPTION_VALUE( key, value )\
	OPTIONS_SERVICE()->testOptionValue( key, value )
}

