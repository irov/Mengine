#pragma once

#include "Kernel/Assertion.h"

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
#   include "Interface/ResourceServiceInterface.h"

#   include "Kernel/Resource.h"

#   include "Config/Char.h"
#   include "Config/ResourceUID.h"
#   include "Config/StdIntTypes.h"
#   include "Config/Typedef.h"

namespace Mengine
{
    namespace Detail
    {
        MENGINE_INLINE void assertionResourceUID( const ResourceUID & _uid, const ConstString & _name, const ConstString & _groupName, const Char * _category, const Char * _file, int32_t _line, const Char * _function )
        {
            if( _uid == INVALID_RESOURCE_UID )
            {
                Helper::Assertion( _category, ASSERTION_LEVEL_FATAL, "ResourceUID != INVALID_RESOURCE_UID", _file, _line, _function, "resource '%s' group '%s' has invalid UID"
                    , _name.c_str()
                    , _groupName.c_str()
                );

                return;
            }

            ResourcePtr uidResource;
            if( RESOURCE_SERVICE()
                ->hasResource( _uid, &uidResource ) == false )
            {
                return;
            }

            Helper::Assertion( _category, ASSERTION_LEVEL_FATAL, "ResourceUID is unique", _file, _line, _function, "resource '%s' group '%s' has duplicate UID %08" MENGINE_PRIx64 "-%04" MENGINE_PRIx64 "-%04" MENGINE_PRIx64 "-%04" MENGINE_PRIx64 "-%012" MENGINE_PRIx64 " already used by resource '%s' group '%s'"
                , _name.c_str()
                , _groupName.c_str()
                , _uid.high >> 32
                , (_uid.high >> 16) & MENGINE_UINT64_C(0xFFFF)
                , _uid.high & MENGINE_UINT64_C(0xFFFF)
                , _uid.low >> 48
                , _uid.low & MENGINE_UINT64_C(0xFFFFFFFFFFFF)
                , uidResource->getName().c_str()
                , uidResource->getGroupName().c_str()
            );
        }
    }
}

#   define MENGINE_ASSERTION_RESOURCE_UID(UID, Name, GroupName) Mengine::Detail::assertionResourceUID(UID, Name, GroupName, MENGINE_CODE_LIBRARY, MENGINE_CODE_FILE, MENGINE_CODE_LINE, MENGINE_CODE_FUNCTION)
#else
#   define MENGINE_ASSERTION_RESOURCE_UID(UID, Name, GroupName)
#endif
