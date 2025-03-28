#pragma once

#include "Interface/DocumentInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        const Char * getStatetracePybind();
        DocumentInterfacePtr createDocumentPybind( const Char * _file, int32_t _line, const Char * _function );
        //////////////////////////////////////////////////////////////////////////
    }
}

//////////////////////////////////////////////////////////////////////////
#define MENGINE_PYBIND_STATETRACE() Mengine::Detail::getStatetracePybind()
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DOCUMENT_ENABLE)
#   define MENGINE_DOCUMENT_PYBIND Mengine::Detail::createDocumentPybind(MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
#else
#   define MENGINE_DOCUMENT_PYBIND nullptr
#endif
//////////////////////////////////////////////////////////////////////////
