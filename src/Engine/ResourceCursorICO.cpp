#include "ResourceCursorICO.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceCursorICO::ResourceCursorICO()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceCursorICO::~ResourceCursorICO()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceCursorICO::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        MemoryInterfacePtr buffer = content->createMemoryFile( false, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer );

        m_buffer = buffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceCursorICO::_release()
    {
        m_buffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & ResourceCursorICO::getBuffer() const
    {
        return m_buffer;
    }
    //////////////////////////////////////////////////////////////////////////
}