#	include "ResourceCursorICO.h"
#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"

#	include "Core/Memory.h"

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

		InputStreamInterfacePtr fileStream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, m_path, false );

		if( fileStream == nullptr )
		{
			return false;
		}

		size_t buffer_size = fileStream->size();

		if( buffer_size == 0 )
		{
			return false;
		}

		m_buffer.resize( buffer_size );

		fileStream->read( &m_buffer[0], buffer_size );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceCursorICO::_release()
	{
		Blobject buffer_deleter;
		m_buffer.swap( buffer_deleter );
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceCursorICO::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	const Blobject & ResourceCursorICO::getBuffer() const
	{
		return m_buffer;
	}
}