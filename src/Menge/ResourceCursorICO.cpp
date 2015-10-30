#	include "ResourceCursorICO.h"

#	include "Interface/FileSystemInterface.h"

#	include "Kernel/ResourceImplement.h"

#	include "Core/MemoryHelper.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceCursorICO );
	//////////////////////////////////////////////////////////////////////////
	ResourceCursorICO::ResourceCursorICO()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCursorICO::_loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceCursorICO * metadata
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceCursorICO *>(_meta);

        metadata->swap_File_Path( m_path );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCursorICO::_compile()
	{
		const ConstString & category = this->getCategory();
		
		m_buffer = Helper::createMemoryFile( m_serviceProvider, category, m_path, false );

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