#pragma once

#include "Config/Config.h"

#if defined(MENGINE_SETJMP_SUPPORTED)
#   include <csetjmp>

#   pragma warning(disable : 4611)

#   define MENGINE_JMP_DECLARE(Var) std::jmp_buf Var
#   define MENGINE_JMP_SET(Var) setjmp(Var)
#   define MENGINE_JMP_JUMP(Var, Code) longjmp(Var, Code)
#else
#   define MENGINE_JMP_DECLARE(Var) 
#   define MENGINE_JMP_SET(Var) 0
#   define MENGINE_JMP_JUMP(Var, Code)
#endif
