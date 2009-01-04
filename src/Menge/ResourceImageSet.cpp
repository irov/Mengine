#	include "ResourceImageSet.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "Utils.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceImageSet );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageSet::ResourceImageSet( const ResourceFactoryParam & _params )
		: ResourceImage( _params )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSet::getMaxSize( std::size_t _frame ) const
	{
		return m_sizes[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSet::getSize( std::size_t _frame ) const
	{
		return m_sizes[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageSet::getCount() const 
	{
		return m_uvs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageSet::getOffset( std::size_t _frame ) const
	{
		return m_imageFrame.offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageSet::getUV( std::size_t _frame ) const
	{
		return m_uvs[ _frame ];
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSet::isAlpha( std::size_t _frame ) const
	{
		assert(!"ResourceImageSet::isAlpha not implemented");
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceImageSet::getImage( std::size_t _frame ) const
	{
		return m_imageFrame.image;
	}
	//////////////////////////////////////////////////////////////////////////
	const String & ResourceImageSet::getFilename( std::size_t _frame ) const
	{
		assert(!"ResourceImageSet::getFilename not implemented");
		return Utils::emptyString();
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageSet::getFilenameCount() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSet::addFrameUV( const mt::vec4f & _uv )
	{
		m_uvs.push_back( _uv );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageSet::loader( XmlElement * _xml )
	{
		ResourceImage::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "File" )
			{
				ImageDesc desc;
				desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
				desc.offset = mt::vec2f(0.f,0.f);
				desc.maxSize = mt::vec2f(0.f,0.f);

				String fileName; 

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", fileName );
					XML_CASE_ATTRIBUTE( "UV", desc.uv );
					XML_CASE_ATTRIBUTE( "Offset", desc.offset );
					XML_CASE_ATTRIBUTE( "MaxSize", desc.maxSize );
				}

				desc.fileName = m_params.category + fileName;

				m_imageDesc = desc;
			}
			XML_CASE_NODE( "Frame" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "UV", &ResourceImageSet::addFrameUV );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageSet::_compile()
	{
		m_imageFrame = loadImageFrame( m_imageDesc.fileName );
		m_imageFrame.uv = m_imageDesc.uv;
		m_imageFrame.maxSize = m_imageDesc.maxSize;
		m_imageFrame.offset = m_imageDesc.offset;

		float ku = m_imageFrame.uv.z - m_imageFrame.uv.x;
		float kv = m_imageFrame.uv.w - m_imageFrame.uv.y;

		m_imageFrame.size = mt::vec2f(m_imageFrame.size.x * ku, m_imageFrame.size.y * kv);

		TVectorSizes::size_type size = m_uvs.size();
		
		if( size == 0 )
		{
			return false;
		}

		m_sizes.resize( size );

		for( TVectorSizes::size_type frame = 0; frame != size; ++frame )
		{
			TVectorUV::value_type & uv = m_uvs[ frame ];

			float u = uv.z - uv.x;
			float v = uv.w - uv.y;

			TVectorSizes::value_type & size = m_sizes[ frame ];

			size.x = m_imageFrame.size.x * u;
			size.y = m_imageFrame.size.y * v;
		}

		return true;
	}
}