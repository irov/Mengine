#pragma once

#include "Kernel/Map.h"
#include "Kernel/ConstString.h"
#include "Kernel/String.h"
#include "Kernel/Factorable.h"

#include "Config/Variant.h"
#include "Config/StdDef.h"

namespace Mengine
{
    typedef std::nullptr_t ParamNull;
    typedef bool ParamBool;
    typedef int64_t ParamInteger;
    typedef double ParamDouble;
    typedef String ParamString;
    typedef WString ParamWString;
    typedef ConstString ParamConstString;
    typedef FactorablePtr ParamFactorablePtr;

    typedef Variant<ParamNull, ParamBool, ParamInteger, ParamDouble, ParamString, ParamWString, ParamConstString, ParamFactorablePtr> ParamVariant;

    typedef Map<ConstString, ParamVariant> Params;
}