#   pragma once

#ifdef _DEBUG

#   include "Config/Char.h"

namespace Menge
{
    void Assertion( const Char* _message, const Char* _file, int _line );
}
#   define MENGINE_ASSERTION(Condition) if((Condition) == false){ Menge::Assertion( #Condition, __FILE__, __LINE__ ); } else {}
#else
#   define MENGINE_ASSERTION(Condition)
#endif
