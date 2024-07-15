#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Interface/ResourceServiceInterface.h"

#   include "Kernel/ResourceCast.h"

#   define MENGINE_ASSERTION_RESOURCE_TYPE( Resource, ResourceType, ... ) MENGINE_ASSERTION_FATAL( (Resource == nullptr || ResourceType::dynamic_from(Resource) != nullptr), __VA_ARGS__)
#   define MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME( ResourceName, ResourceType, Ret, ... )\
    if( RESOURCE_SERVICE()->hasResource(ConstString::none(), ResourceName, false, nullptr) == true )\
    {\
        if( Helper::dynamicResourceCast<ResourceType>(RESOURCE_SERVICE()->getResourceReference(ConstString::none(), ResourceName)) == nullptr )\
        {\
            Mengine::Helper::Assertion(MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_FATAL, #ResourceType, MENGINE_CODE_FILE, MENGINE_CODE_LINE, __VA_ARGS__);\
            return Ret;\
        }\
    }
#   define MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME_VOID( ResourceName, ResourceType, Ret, ... )\
    if( RESOURCE_SERVICE()->hasResource(ConstString::none(), ResourceName, false, nullptr) == true )\
    {\
        if( Helper::dynamicResourceCast<ResourceType>(RESOURCE_SERVICE()->getResourceReference(ConstString::none(), ResourceName)) == nullptr )\
        {\
            Mengine::Helper::Assertion(MENGINE_CODE_LIBRARY, Mengine::ASSERTION_LEVEL_FATAL, #ResourceType, MENGINE_CODE_FILE, MENGINE_CODE_LINE, __VA_ARGS__);\
            return;\
        }\
    }
#else
#   define MENGINE_ASSERTION_RESOURCE_TYPE( Resource, ResourceType, ... )
#   define MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME( ResourceName, ResourceType, Ret, ... )
#   define MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME_VOID( ResourceName, ResourceType, Ret, ... )
#endif