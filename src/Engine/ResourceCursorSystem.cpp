#include "ResourceCursorSystem.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceCursorSystem::ResourceCursorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceCursorSystem::~ResourceCursorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & ResourceCursorSystem::getBuffer() const
    {
        return MemoryInterfacePtr::none();
    }
}