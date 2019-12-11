#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/Document.h"

namespace Mengine
{
    class DocumentServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "DocumentService" )

    public:
        virtual DocumentPtr createDocument( const DocumentPtr & _parent, const Char * _modulePath, const Char * _file, const Char * _function, uint32_t _line, const Char * _format, ... ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define DOCUMENT_SERVICE()\
    ((Mengine::DocumentServiceInterface*)SERVICE_GET(Mengine::DocumentServiceInterface))
//////////////////////////////////////////////////////////////////////////
