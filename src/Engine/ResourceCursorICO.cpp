#include "ResourceCursorICO.h"

#include "Interface/FileServiceInterface.h"

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

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        m_buffer = Helper::createMemoryFile( fileGroup, filePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

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