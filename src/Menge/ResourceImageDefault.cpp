#	include "ResourceImageDefault.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageDefault )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault::ResourceImageDefault( const ResourceFactoryParam & _params )
		: ResourceImage( _params )
//		, m_offset( 0.f, 0.f )
//		, m_uv( 0.f, 0.f, 1.f, 1.f )
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
		//return m_vectorImageFrames[ _frame ].size;
		//return m_vectorAtlasMaxSizes[ _frame ];
		return m_vectorImageFrames[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getOffset( unsigned int _frame ) const
	{
		//return m_vectorAtlasOffsets[_frame];
		return m_vectorImageFrames[ _frame ].offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDefault::getUV( unsigned int _frame ) const
	{
		//return m_vectorAtlasUVs[_frame];
		return m_vectorImageFrames[ _frame ].uv;
		//return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceImageDefault::getImage( unsigned int _frame ) const
	{
		return m_vectorImageFrames[ _frame ].image;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::addFrameFile( const std::string & _path )
	{
		m_vectorFileNames.push_back( m_params.category + _path );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::addFrameAtlasUV( const mt::vec4f & _v )
	{
		m_vectorAtlasUVs.push_back( _v );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::addFrameAtlasOffset( const mt::vec2f & _v )
	{
		m_vectorAtlasOffsets.push_back( _v );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::addFrameAtlasMaxSize( const mt::vec2f & _v )
	{
		m_vectorAtlasMaxSizes.push_back( _v );
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
					XML_CASE_ATTRIBUTE_MEMBER( "UV", &ResourceImageDefault::addFrameAtlasUV );
					XML_CASE_ATTRIBUTE_MEMBER( "Offset", &ResourceImageDefault::addFrameAtlasOffset );
					XML_CASE_ATTRIBUTE_MEMBER( "MaxSize", &ResourceImageDefault::addFrameAtlasMaxSize );
				}				
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		int i = 0;
		for( TVectorFileNames::iterator
			it = m_vectorFileNames.begin(),
			it_end = m_vectorFileNames.end();
		it != it_end;
		++it)
		{
			ImageFrame frame = loadImageFrame( *it );

			frame.uv = m_vectorAtlasUVs[i];

			float u = frame.uv.z - frame.uv.x;
			float v = frame.uv.w - frame.uv.y;

			frame.size = mt::vec2f(frame.size.x * u,frame.size.y * v);;
			frame.offset =  m_vectorAtlasOffsets[i++];

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
		m_vectorAtlasUVs.clear();
		m_vectorAtlasOffsets.clear();
	}
}