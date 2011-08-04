#	include "ResourceCursorICO.h"
#	include "ResourceImplement.h"

#	include "BinParser.h"
#	include "LoaderEngine.h"

#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceCursorICO );
	//////////////////////////////////////////////////////////////////////////
	void ResourceCursorICO::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::File_Path, m_path );
		}
	}

	const ConstString& ResourceCursorICO::getPath() const
	{
		return m_path;
	}
}