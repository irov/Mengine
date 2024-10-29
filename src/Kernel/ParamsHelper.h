#pragma once

#include "Kernel/Params.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/IntrusivePtr.h"
#include "Kernel/FactorablePointer.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        ParamBool getParam( const Params & _params, const ConstString & _key, ParamBool _default );
        ParamInteger getParam( const Params & _params, const ConstString & _key, ParamInteger _default );
        void setParam( Params & _params, const ConstString & _key, ParamInteger _value );
        ParamDouble getParam( const Params & _params, const ConstString & _key, ParamDouble _default );
        String getParam( const Params & _params, const ConstString & _key, const Char * _default );
        WString getParam( const Params & _params, const ConstString & _key, const WChar * _default );
        ConstString getParam( const Params & _params, const ConstString & _key, const ConstString & _default );
        FactorablePointer getParam( const Params & _params, const ConstString & _key, const FactorablePtr & _default );
        void setParam( Params & _params, const ConstString & _key, const FactorablePtr & _value );
        //////////////////////////////////////////////////////////////////////////
    }
}