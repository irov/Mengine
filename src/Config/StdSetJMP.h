#pragma once

#include "Config/Config.h"

#if defined(MENGINE_SETJMP_SUPPORTED)
#   include <csetjmp>

#   pragma warning(disable : 4611)

#   define MENGINE_JMP_DECLARE(Var) std::jmp_buf Var
#   define MENGINE_JMP_SET(Var) setjmp((Var))
#   define MENGINE_JMP_JUMP(Var, Code) std::longjmp((Var), (Code))

namespace Mengine
{
    struct MENGINE_ALIGNAS(16) MENGINE_JMPBUF
    {
        MENGINE_JMP_DECLARE( value );
    };
}

#   define MENGINE_JMPBUF_DECLARE(Var) Mengine::MENGINE_JMPBUF * Var = nullptr
#   define MENGINE_JMPBUF_ALLOCATE(Var) Var = Helper::newMemoryT<MENGINE_JMPBUF>( "jmp" )
#   define MENGINE_JMPBUF_DEALLOCATE(Var) Helper::deleteMemoryT( Var, "jmp" )

#else
#   define MENGINE_JMP_DECLARE(Var) 
#   define MENGINE_JMP_SET(Var) 0
#   define MENGINE_JMP_JUMP(Var, Code)

#   define MENGINE_JMPBUF_DECLARE(Var)
#   define MENGINE_JMPBUF_ALLOCATE(Var)
#   define MENGINE_JMPBUF_DEALLOCATE(Var)
#endif
