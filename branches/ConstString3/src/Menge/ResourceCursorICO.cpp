#	include "ResourceCursorICO.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"

#	include "BinParser.h"
#	include "LoaderEngine.h"

#	include "LogEngine.h"

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
	void ResourceCursorICO::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::File_Path, m_path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCursorICO::_compile()
	{
		const ConstString & category = this->getCategory();

		FileInputStreamInterface * fileStream = FileEngine::get()
			->openInputFile( category, m_path );

		if( fileStream == 0 )
		{
			return false;
		}

		m_bufferSize = fileStream->size();

		m_buffer = new char [m_bufferSize];

		fileStream->read( m_buffer, m_bufferSize );

		fileStream->close();

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
	const WString& ResourceCursorICO::getPath() const
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