#	include "ResourceBinary.h"
#	include "ResourceImplement.h"

#	include "BinParser.h"
#	include "FileEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceBinary );
	//////////////////////////////////////////////////////////////////////////
	const TBlobject & ResourceBinary::getBlobject() const
	{
		return m_blobject;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceBinary::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::File_Path, m_path );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceBinary::_compile()
	{
		const ConstString & category = this->getCategory();

		FileInputInterface * file = FileEngine::get()
			->openInputFile( category, m_path.str() );

		if( file == 0 )
		{
			return false;
		}

		std::size_t size = file->size();

		if( size == 0 )
		{
			return true;
		}

		m_blobject.resize( size );
		file->read( &m_blobject.front(), size );

		return true;
	}
}