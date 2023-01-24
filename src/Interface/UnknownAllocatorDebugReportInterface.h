#pragma once

#include "Interface/UnknownInterface.h"

#include "Config/Char.h"
#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class UnknownAllocatorDebugReportInterface
        : public UnknownInterface
    {
    public:
        virtual void blockAllocator( const Char * _doc ) = 0;

    public:
        virtual uint32_t getAllocatorReportCount() const = 0;
        virtual size_t getAllocatorReportInfo( uint32_t _index, const Char ** _doc ) const = 0;

    public:
        virtual size_t findAllocatorReportInfo( const Char * _doc ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<UnknownAllocatorDebugReportInterface> UnknownAllocatorDebugReportInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}