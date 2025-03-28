#pragma once

#include "Interface/ServiceInterface.h"

#include "Interface/DocumentInterface.h"

namespace Mengine
{
    class DocumentServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "DocumentService" )

    public:
        virtual DocumentInterfacePtr createDocument( const DocumentInterfacePtr & _parent, const Char * _modulePath, const Char * _file, int32_t _line, const Char * _function, const Char * _format, ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 7, 8 ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define DOCUMENT_SERVICE()\
    ((Mengine::DocumentServiceInterface *)SERVICE_GET(Mengine::DocumentServiceInterface))
//////////////////////////////////////////////////////////////////////////
