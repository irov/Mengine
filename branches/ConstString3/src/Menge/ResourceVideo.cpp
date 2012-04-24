#	include "ResourceVideo.h"

#	include "ResourceImplement.h"

#	include "BinParser.h"

#	include "Consts.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceVideo );
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo::ResourceVideo()
		: m_alpha(false)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceVideo::~ResourceVideo()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::loader( BinParser * _parser )
	{
		ResourceReference::loader( _parser );

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::File_Path, m_filePath );
			BIN_CASE_ATTRIBUTE( Protocol::File_Alpha, m_alpha );
			BIN_CASE_ATTRIBUTE( Protocol::File_Codec, m_codecType );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::_compile()
	{
		if( m_filePath.empty() == true )
		{
			return false;
		}
		
		if( m_codecType.empty() == true )
		{
			MENGE_LOG_ERROR( "ResourceVideo: you must determine codec for file '%S'"
				, m_filePath.c_str()
				);

			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceVideo::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceVideo::isAlpha() const
	{
		return m_alpha;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & ResourceVideo::getFilePath() const
	{
		return m_filePath;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceVideo::getCodecType() const
	{
		return m_codecType;
	}
	//////////////////////////////////////////////////////////////////////////
}
