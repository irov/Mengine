#	include "ResourceImageDefault.h"
#	include "ResourceImplement.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageDefault )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault::ResourceImageDefault( const std::string & _name )
		: ResourceImage( _name )
		, m_offset(0.f,0.f)
		, m_uv(0.f,0.f,1.f,1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getMaxSize( size_t _frame ) const
	{
		return m_frames[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageDefault::getCount()
	{
		return m_frames.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getSize( size_t _frame ) const
	{
		return m_frames[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getOffset( size_t _frame ) const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDefault::getUV( size_t _frame ) const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceImageDefault::getImage( size_t _frame ) const
	{
		return m_frames[ _frame ].image;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::loader( TiXmlElement * _xml )
	{
		ResourceImage::loader( _xml );

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE("File")
			{
				XML_DEF_ATTRIBUTES_NODE(Path);
				m_filesImages.push_back(Path);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		std::vector<char> buff;
		for each( const std::string & file in m_filesImages )
		{
			ImageFrame frame = loadImageFrame( file, buff );

			m_frames.push_back( frame );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::_release()
	{
		for each( const ImageFrame & frame in m_frames )
		{
			releaseImageFrame( frame );
		}

		m_frames.clear();
	}
}