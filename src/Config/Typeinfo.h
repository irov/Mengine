#pragma once

#include <typeinfo>
#include <typeindex>

#define MENGINE_TYPEID(T) typeid(T)

#define MENGINE_TYPEINFO_NAME(T) (typeid(T).name())
#define MENGINE_TYPEINFO_INDEX(T) (std::type_index(typeid(T)))
