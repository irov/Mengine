#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        bool OpenALErrorCheck( const Char * _method, const Char * _file, uint32_t _line );
    }
}
//////////////////////////////////////////////////////////////////////////
#define OPENAL_CALL(Method, Args)\
    (Method Args, Mengine::Helper::OpenALErrorCheck( #Method, MENGINE_CODE_FILE, MENGINE_CODE_LINE ))
//////////////////////////////////////////////////////////////////////////
#define IF_OPENAL_CALL(Method, Args)\
    if((Method Args, Mengine::Helper::OpenALErrorCheck( #Method, MENGINE_CODE_FILE, MENGINE_CODE_LINE )) == false)
//////////////////////////////////////////////////////////////////////////
#define RET_OPENAL_CALL(Type, Ret, Method, Args)\
    Type Ret = Method Args; Mengine::Helper::OpenALErrorCheck( #Method, MENGINE_CODE_FILE, MENGINE_CODE_LINE )
//////////////////////////////////////////////////////////////////////////

