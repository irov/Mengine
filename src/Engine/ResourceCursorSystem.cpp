#include "ResourceCursorSystem.h"

#include "Metacode/Metacode.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceCursorSystem::ResourceCursorSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceCursorSystem::_loader( const Metabuf::Metadata * _meta )
    {
        const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorSystem * metadata
            = static_cast<const Metacode::Meta_Data::Meta_DataBlock::Meta_ResourceCursorSystem *>(_meta);

        m_path = metadata->get_File_Path();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceCursorSystem::getPath() const
    {
        return m_path;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & ResourceCursorSystem::getBuffer() const
    {
        return m_buffer;
    }
}