#pragma once

#include "Interface/DocumentInterface.h"

#include "Config/Char.h"

namespace Mengine
{
    namespace Detail
    {
        DocumentInterfacePtr createPythonDocument( const Char * _file, int32_t _line, const Char * _function );
    }
}

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_DOCUMENT_ENABLE)
#   define MENGINE_DOCUMENT_PYTHON Mengine::Detail::createPythonDocument(MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
#else
#   define MENGINE_DOCUMENT_PYTHON nullptr
#endif
//////////////////////////////////////////////////////////////////////////
