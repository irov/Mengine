#pragma once

#include "Config/StdDef.h"

extern "C"
{
    bool mengineLocalStorageGetItem( const char * _key, char * _value, size_t _capacity );
    void mengineLocalStorageSetItem( const char * _key, const char * _value );
}
