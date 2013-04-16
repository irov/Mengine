#	include "ResourceCursorICO.h"
#	include "Kernel/ResourceImplement.h"

#	include "Interface/FileSystemInterface.h"

#	include "Metacode.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceCursorICO );
	//////////////////////////////////////////////////////////////////////////
	ResourceCursorICO::ResourceCursorICO()
		: m_buffer(0)
		, m_bufferSize(0)
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

		InputStreamInterface * fileStream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( category, m_path );

		if( fileStream == 0 )
		{
			return false;
		}

		m_bufferSize = fileStream->size();

		m_buffer = new char [m_bufferSize];

		fileStream->read( m_buffer, m_bufferSize );

		fileStream->destroy();
        fileStream = NULL;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceCursorICO::_release()
	{
		delete [] m_buffer;
		m_buffer = 0;
		m_bufferSize = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & ResourceCursorICO::getPath() const
	{
		return m_path;
	}
	//////////////////////////////////////////////////////////////////////////
	void * ResourceCursorICO::getBuffer( size_t & _size ) const
	{
		_size = m_bufferSize;

		return m_buffer;
	}
}