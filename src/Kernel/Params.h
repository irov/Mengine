#pragma once

#include "Kernel/Map.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Variant.h"

namespace Mengine
{
    typedef nullptr_t ParamNull;
    typedef bool ParamBool;
    typedef int64_t ParamInteger;
    typedef double ParamDouble;
    typedef String ParamString;
    typedef WString ParamWString;
    typedef ConstString ParamConstString;

    typedef Variant<ParamNull, ParamBool, ParamInteger, ParamDouble, ParamString, ParamWString, ParamConstString> ParamVariant;

    typedef Map<ConstString, ParamVariant> Params;
}