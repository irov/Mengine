#include "ResourceCursorICO.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/MemoryHelper.h"
#include "Kernel/Document.h"
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
    void ResourceCursorICO::setFilePath( const FilePath & _filePath )
    {
        m_filePath = _filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceCursorICO::_compile()
    {
        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        m_buffer = Helper::createMemoryFile( fileGroup, m_filePath, false, MENGINE_DOCUMENT_FUNCTION );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceCursorICO::_release()
    {
        m_buffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceCursorICO::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & ResourceCursorICO::getBuffer() const
    {
        return m_buffer;
    }
}