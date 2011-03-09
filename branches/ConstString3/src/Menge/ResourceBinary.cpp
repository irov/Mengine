#	include "ResourceBinary.h"
#	include "ResourceImplement.h"

#	include "BinParser.h"
#	include "LoaderEngine.h"

#	include "Utils/Logger/Logger.h"

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

		bool exist = false;

		if( LoaderEngine::get()
			->import( category, Helper::to_str(m_path), m_blobject, exist ) == false )
		{
			if( exist == false )
			{
				MENGE_LOG_ERROR("ResourceBinary: '%s' invalid compile bin, miss binary file '%s'"
					, m_name.c_str()
					, m_path.c_str()
					);
			}
			else
			{
				MENGE_LOG_ERROR("ResourceBinary: '%s' invalid compile bin, invalid binary file '%s'"
					, m_name.c_str()
					, m_path.c_str()
					);
			}

			return false;
		}

		return true;
	}
}