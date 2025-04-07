#pragma once

#include "Interface/OptionsServiceInterface.h"

//////////////////////////////////////////////////////////////////////////
#define HAS_OPTION( Key )\
    ([=](){static bool value = OPTIONS_SERVICE()->hasOption( Key, false ); return value;}())
//////////////////////////////////////////////////////////////////////////
#define HAS_OPTION_WITH_VALUE( Key )\
    ([=](){static bool value = OPTIONS_SERVICE()->hasOption( Key, true ); return value;}())
//////////////////////////////////////////////////////////////////////////
#define HAS_OPTION_VALUE( Key, Value )\
    ([=](const Char ** _value){static bool exist = OPTIONS_SERVICE()->hasOption( Key, true ); if( exist == false ){return false;} static const Mengine::Char * value = OPTIONS_SERVICE()->getOptionValue( Key, "" ); *_value = value; return true;}(Value))
//////////////////////////////////////////////////////////////////////////
#define GET_OPTION_VALUE( Key, Default )\
    ([=](){static const Mengine::Char * value = OPTIONS_SERVICE()->getOptionValue( Key, Default ); return value;}())
//////////////////////////////////////////////////////////////////////////
#define GET_OPTION_VALUE_INT32( Key, Default )\
    ([=](){static int32_t value = OPTIONS_SERVICE()->getOptionInt32( Key, Default ); return value;}())
//////////////////////////////////////////////////////////////////////////
#define GET_OPTION_VALUE_UINT32( Key, Default )\
    ([=](){static uint32_t value = OPTIONS_SERVICE()->getOptionUInt32( Key, Default ); return value;}())
//////////////////////////////////////////////////////////////////////////
#define TEST_OPTION_VALUE( Key, Value )\
    OPTIONS_SERVICE()->testOptionValue( Key, Value )
//////////////////////////////////////////////////////////////////////////
