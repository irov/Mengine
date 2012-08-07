#	include "ResourceVideo.h"

#	include "ResourceImplement.h"

#	include "Metacode.h"

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
	void ResourceVideo::loader( const Metabuf::Metadata * _meta )
	{
        const Metacode::Meta_DataBlock::Meta_ResourceVideo * metadata 
            = static_cast<const Metacode::Meta_DataBlock::Meta_ResourceVideo *>(_meta);

        metadata->swap_File_Path( m_filePath );
        metadata->swap_File_Codec( m_codecType );

        metadata->get_File_Alpha( m_alpha );
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
