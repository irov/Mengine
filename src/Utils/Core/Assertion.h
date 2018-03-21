#   pragma once

#ifdef _DEBUG

#   include "Config/Char.h"

namespace Mengine
{
    void Assertion( const Char* _message, const Char* _file, int _line );
}
#   define MENGINE_ASSERTION(Condition) if((Condition) == false){ Mengine::Assertion( #Condition, __FILE__, __LINE__ ); } else {}
#else
#   define MENGINE_ASSERTION(Condition)
#endif
