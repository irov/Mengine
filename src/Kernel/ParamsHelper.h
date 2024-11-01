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
        void setParam( Params & _params, const ConstString & _key, ParamBool _value );
        ParamInteger getParam( const Params & _params, const ConstString & _key, ParamInteger _default );
        void setParam( Params & _params, const ConstString & _key, ParamInteger _value );
        ParamDouble getParam( const Params & _params, const ConstString & _key, ParamDouble _default );
        void setParam( Params & _params, const ConstString & _key, ParamDouble _value );
        String getParam( const Params & _params, const ConstString & _key, const Char * _default );
        void setParam( Params & _params, const ConstString & _key, const String & _value );
        WString getParam( const Params & _params, const ConstString & _key, const WChar * _default );
        void setParam( Params & _params, const ConstString & _key, const WString & _value );
        ConstString getParam( const Params & _params, const ConstString & _key, const ConstString & _default );
        void setParam( Params & _params, const ConstString & _key, const ConstString & _value );
        FilePath getParam( const Params & _params, const ConstString & _key, const FilePath & _default );
        void setParam( Params & _params, const ConstString & _key, const FilePath & _value );
        FactorablePointer getParam( const Params & _params, const ConstString & _key, const FactorablePtr & _default );
        void setParam( Params & _params, const ConstString & _key, const FactorablePtr & _value );
        //////////////////////////////////////////////////////////////////////////
    }
}