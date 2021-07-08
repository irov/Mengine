#pragma once

#include "Config/Config.h"

#include <typeinfo>
#include <typeindex>

#ifndef MENGINE_TYPEID
#define MENGINE_TYPEID(T) typeid(T)
#endif

#ifndef MENGINE_TYPEINFO_NAME
#define MENGINE_TYPEINFO_NAME(T) (typeid(T).name())
#endif

#ifndef MENGINE_TYPEINFO_INDEX
#define MENGINE_TYPEINFO_INDEX(T) (std::type_index(typeid(T)))
#endif
