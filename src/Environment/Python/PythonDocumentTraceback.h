#pragma once

#include "Interface/DocumentInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * getTracebackPybind();
        DocumentInterfacePtr createDocumentPybind( const Char * _file, const Char * _function, uint32_t _line );
        //////////////////////////////////////////////////////////////////////////
    }
}

//////////////////////////////////////////////////////////////////////////
#define MENGINE_PYBIND_TRACEBACK() Mengine::Detail::getTracebackPybind()
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DOCUMENT_ENABLE)
#   define MENGINE_DOCUMENT_PYBIND Mengine::Detail::createDocumentPybind(MENGINE_CODE_FILE, MENGINE_CODE_FUNCTION, MENGINE_CODE_LINE)
#else
#   define MENGINE_DOCUMENT_PYBIND nullptr
#endif
//////////////////////////////////////////////////////////////////////////