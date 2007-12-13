#	include "ResourceImageDefault.h"

#	include "ResourceImplement.h"

#	include "XmlParser/XmlParser.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageDefault )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault::ResourceImageDefault( const std::string & _name )
		: ResourceImage( _name )
		, m_offset( 0.f, 0.f )
		, m_uv( 0.f, 0.f, 1.f, 1.f )
		, m_filter( 1 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getMaxSize( size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageDefault::getCount() const
	{
		return m_vectorImageFrames.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getSize( size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].size;
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
		return m_vectorImageFrames[ _frame ].image;
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
				m_vectorFileNames.push_back(Path);
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		std::vector<char> buff;

		for( TVectorFileNames::iterator
			it = m_vectorFileNames.begin(),
			it_end = m_vectorFileNames.end();
		it != it_end;
		++it)
		{
			ImageFrame frame = loadImageFrame( *it, buff );

			m_vectorImageFrames.push_back( frame );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::_release()
	{
		for( TVectorImageFrame::iterator
			it = m_vectorImageFrames.begin(),
			it_end = m_vectorImageFrames.end();
		it != it_end;
		++it)
		{
			releaseImageFrame( *it );
		}

		m_vectorImageFrames.clear();
	}
}