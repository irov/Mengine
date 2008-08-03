#	include "ResourceImageDefault.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageDefault )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault::ResourceImageDefault( const ResourceFactoryParam & _params )
		: ResourceImage( _params )
		, m_filter( 1 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getMaxSize( unsigned int _frame ) const
	{
		return m_vectorImageFrames[ _frame ].maxSize;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageDefault::getCount() const
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
		return m_vectorImageFrames[ _frame ].offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDefault::getUV( unsigned int _frame ) const
	{
		return m_vectorImageFrames[ _frame ].uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceImageDefault::getImage( unsigned int _frame ) const
	{
		return m_vectorImageFrames[ _frame ].image;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::isAlpha( unsigned int _frame ) const
	{
		return m_vectorImageFrames[ _frame ].isAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::loader( XmlElement * _xml )
	{
		ResourceImage::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("File")
			{
				ImageDesc desc;
				desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
				desc.offset = mt::vec2f(0.f,0.f);
				desc.maxSize = mt::vec2f(-1.f,-1.f);
				desc.size = mt::vec2f(-1.f,-1.f);
				desc.isAlpha = true; //

				std::string fileName; 

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", fileName );
					XML_CASE_ATTRIBUTE( "UV", desc.uv );
					XML_CASE_ATTRIBUTE( "Offset", desc.offset );
					XML_CASE_ATTRIBUTE( "MaxSize", desc.maxSize );
					XML_CASE_ATTRIBUTE( "Size", desc.size );
					XML_CASE_ATTRIBUTE( "Alpha", desc.isAlpha );
				}

				desc.fileName = m_params.category + fileName;

				m_vectorImageDescs.push_back( desc );
			}
			XML_CASE_NODE("Files")
			{
				ImageDesc desc;
				desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
				desc.offset = mt::vec2f(0.f,0.f);
				desc.maxSize = mt::vec2f(-1.f,-1.f);
				desc.size = mt::vec2f(-1.f,-1.f);
				desc.isAlpha = true; //

				std::string fileName; 

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", fileName );
				}

			}

		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		for( TVectorImageDesc::iterator
			it = m_vectorImageDescs.begin(),
			it_end = m_vectorImageDescs.end();
		it != it_end;
		++it)
		{
			ImageFrame frame = loadImageFrame( it->fileName );


			if( frame.image == NULL )
			{
				return false;
			}

			frame.uv = it->uv;
			
			frame.maxSize = it->maxSize;
			frame.offset =  it->offset;

			//float ku = frame.uv.z - frame.uv.x;
			//float kv = frame.uv.w - frame.uv.y;
			//mt::vec2f(frame.size.x * ku , frame.size.y * kv );

			if( frame.maxSize.x < 0.f || frame.maxSize.y < 0.f )
			{
				frame.maxSize = frame.size;
			}
			else
			{
				frame.size = it->size;
			}

			frame.isAlpha = it->isAlpha;


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
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::addImagePath( const String& _imagePath )
	{
		ImageDesc desc;
		desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
		desc.offset = mt::vec2f(0.f,0.f);
		desc.maxSize = mt::vec2f(-1.f,-1.f);
		desc.size = mt::vec2f(-1.f,-1.f);
		desc.isAlpha = true; //
		desc.fileName = _imagePath;

		m_vectorImageDescs.push_back( desc );
	}
}