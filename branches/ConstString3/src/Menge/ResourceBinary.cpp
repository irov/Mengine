#	include "ResourceBinary.h"
#	include "ResourceImplement.h"

#	include "BinParser.h"
#	include "LoaderEngine.h"

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

		if( LoaderEngine::get()
			->import( category, to_str(m_path), m_blobject ) == false )
		{
			return false;
		}

		return true;
	}
}