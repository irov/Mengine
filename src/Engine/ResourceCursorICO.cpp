#include "ResourceCursorICO.h"

#include "Metacode/Metacode.h"

#include "Interface/FileSystemInterface.h"

#include "Kernel/MemoryHelper.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceCursorICO::ResourceCursorICO()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceCursorICO::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorICO * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorICO *>(_meta);

        m_path = metadata->get_File_Path();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceCursorICO::_compile()
    {
        const FileGroupInterfacePtr & category = this->getCategory();

        m_buffer = Helper::createMemoryFile( category, m_path, false, "ResourceCursorICO", __FILE__, __LINE__ );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceCursorICO::_release()
    {
        m_buffer = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceCursorICO::getPath() const
    {
        return m_path;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & ResourceCursorICO::getBuffer() const
    {
        return m_buffer;
    }
}