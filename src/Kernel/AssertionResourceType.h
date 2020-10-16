#pragma once

#include "Kernel/Assertion.h"

//////////////////////////////////////////////////////////////////////////
#if MENGINE_ASSERTION_DEBUG
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_ASSERTION_RESOURCE_TYPE( Resource, ResourceType, ... ) MENGINE_ASSERTION_FATAL( (Resource != nullptr && stdex::intrusive_dynamic_cast<ResourceType>(Resource) != nullptr), __VA_ARGS__)
//////////////////////////////////////////////////////////////////////////
#   include "Interface/ResourceServiceInterface.h"
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME( ResourceName, ResourceType, Ret, ... )\
    if( RESOURCE_SERVICE()->hasResource(ResourceName, nullptr) == true )\
    {\
        if( Helper::dynamicResourceCast<ResourceType>(RESOURCE_SERVICE()->getResourceReference(ResourceName)) == nullptr )\
        {\
            Mengine::Helper::Assertion(Mengine::ASSERTION_LEVEL_FATAL, #ResourceType, MENGINE_CODE_FILE, MENGINE_CODE_LINE, __VA_ARGS__);\
            return Ret;\
        }\
    }
//////////////////////////////////////////////////////////////////////////
#   define MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME_VOID( ResourceName, ResourceType, Ret, ... )\
    if( RESOURCE_SERVICE()->hasResource(ResourceName, nullptr) == true )\
    {\
        if( Helper::dynamicResourceCast<ResourceType>(RESOURCE_SERVICE()->getResourceReference(ResourceName)) == nullptr )\
        {\
            Mengine::Helper::Assertion(Mengine::ASSERTION_LEVEL_FATAL, #ResourceType, MENGINE_CODE_FILE, MENGINE_CODE_LINE, __VA_ARGS__);\
            return;\
        }\
    }\
//////////////////////////////////////////////////////////////////////////
#else
#   define MENGINE_ASSERTION_RESOURCE_TYPE( Resource, ResourceType, ... )
#   define MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME( ResourceName, ResourceType, Ret, ... )
#   define MENGINE_ASSERTION_RESOURCE_TYPE_BY_NAME_VOID( ResourceName, ResourceType, Ret, ... )
#endif
//////////////////////////////////////////////////////////////////////////