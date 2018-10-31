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
	ResourceCursorSystem::~ResourceCursorSystem()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceCursorSystem::setFilePath( const FilePath & _filePath )
	{
		m_filePath = _filePath;
	}
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceCursorSystem::getFilePath() const
    {
        return m_filePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const MemoryInterfacePtr & ResourceCursorSystem::getBuffer() const
    {
        return m_buffer;
    }
}