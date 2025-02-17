#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        bool OpenALErrorCheck( const Char * _method, const Char * _file, uint32_t _line, const Char * _function );
    }
}
//////////////////////////////////////////////////////////////////////////
#define MENGINE_OPENAL_CALL(Method, Args)\
    (Method Args, Mengine::Helper::OpenALErrorCheck( #Method, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION ))
//////////////////////////////////////////////////////////////////////////
#define MENGINE_IF_OPENAL_CALL(Method, Args)\
    if((Method Args, Mengine::Helper::OpenALErrorCheck( #Method, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION )) == false)
//////////////////////////////////////////////////////////////////////////
#define MENGINE_RET_OPENAL_CALL(Type, Ret, Method, Args)\
    Type Ret = Method Args; Mengine::Helper::OpenALErrorCheck( #Method, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION )
//////////////////////////////////////////////////////////////////////////

