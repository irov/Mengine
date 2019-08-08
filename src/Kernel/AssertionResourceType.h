#pragma once

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/Assertion.h"

#ifdef MENGINE_ASSERTION_DEBUG
#   define MENGINE_ASSERTION_RESOURCE_TYPE( ResourceName, ResourceType, Ret, Message, ... )\
    if( RESOURCE_SERVICE()->hasResource(ResourceName, nullptr) == true )\
    {\
        const ResourcePtr & resource = RESOURCE_SERVICE()->getResourceReference(ResourceName);\
        if( dynamic_cast<ResourceType>(resource.get()) == nullptr )\
        {\
            Helper::Assertion(ASSERTION_LEVEL_FATAL, #ResourceType, MENGINE_CODE_FILE, MENGINE_CODE_LINE, Message, __VA_ARGS__);\
            return Ret;\
        }\
    }

#   define MENGINE_ASSERTION_RESOURCE_TYPE_VOID( ResourceName, ResourceType, Ret, Message, ... )\
    if( RESOURCE_SERVICE()->hasResource(ResourceName, nullptr) == true )\
    {\
        const ResourcePtr & resource = RESOURCE_SERVICE()->getResourceReference(ResourceName);\
        if( dynamic_cast<ResourceType>(resource.get()) == nullptr )\
        {\
            Helper::Assertion(ASSERTION_LEVEL_FATAL, #ResourceType, MENGINE_CODE_FILE, MENGINE_CODE_LINE, Message, __VA_ARGS__);\
            return;\
        }\
    }\

#else
#   define MENGINE_ASSERTION_RESOURCE_TYPE( ResourceName, ResourceType, Ret, Message, ... )
#   define MENGINE_ASSERTION_RESOURCE_TYPE_VOID( ResourceName, ResourceType, Ret, Message, ... )
#endif