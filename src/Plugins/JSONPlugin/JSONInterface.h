#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/InputStreamInterface.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceJSONInterface
        : public UnknownInterface
    {
    public:
        virtual const jpp::object & getJSON() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class JSONServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "JSONService" );

    public:
        virtual bool loadJSON( const InputStreamInterfacePtr & _stream, jpp::object * _json, const Char * _doc ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define JSON_SERVICE()\
	((Mengine::JSONServiceInterface *)SERVICE_GET(Mengine::JSONServiceInterface))
//////////////////////////////////////////////////////////////////////////