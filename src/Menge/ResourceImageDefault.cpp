#	include "ResourceImageDefault.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

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
	const mt::vec2f & ResourceImageDefault::getMaxSize( unsigned int _frame ) const
	{
		return m_vectorImageFrames[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ResourceImageDefault::getCount() const
	{
		return m_vectorImageFrames.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getSize( unsigned int _frame ) const
	{
		return m_vectorImageFrames[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getOffset( unsigned int _frame ) const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDefault::getUV( unsigned int _frame ) const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceImageDefault::getImage( unsigned int _frame ) const
	{
		return m_vectorImageFrames[ _frame ].image;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::addFrameFile( const std::string & _path )
	{
		m_vectorFileNames.push_back( _path );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::loader( XmlElement * _xml )
	{
		ResourceImage::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("File")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "Path", &ResourceImageDefault::addFrameFile );
				}				
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		for( TVectorFileNames::iterator
			it = m_vectorFileNames.begin(),
			it_end = m_vectorFileNames.end();
		it != it_end;
		++it)
		{
			ImageFrame frame = loadImageFrame( *it );

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