#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/ContentInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/JSON.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class JSONServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "JSONService" )

    public:
        virtual bool load( const ContentInterfacePtr & _content, jpp::object * const _obj, const DocumentInterfacePtr & _doc ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceJSONInterface
        : public UnknownInterface
    {
    public:
        virtual const jpp::object & getJSON() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
#define JSON_SERVICE()\
    ((Mengine::JSONServiceInterface *)SERVICE_GET(Mengine::JSONServiceInterface))
//////////////////////////////////////////////////////////////////////////