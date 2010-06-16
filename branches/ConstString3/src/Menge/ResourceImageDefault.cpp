#	include "ResourceImageDefault.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"
#	include "Utils/Core/String.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageDefault )
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDefault::ResourceImageDefault()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getMaxSize( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].maxSize;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageDefault::getCount() const
	{
		return m_vectorImageFrames.size();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getSize( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDefault::getOffset( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDefault::getUV( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].uv;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceImageDefault::getTexture( std::size_t _frame )
	{
		return m_vectorImageFrames[ _frame ].texture;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::isAlpha( std::size_t _frame ) const
	{
		return m_vectorImageFrames[ _frame ].isAlpha;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageDefault::getFilename( std::size_t _frame ) const
	{
		return m_vectorImageDescs[ _frame ].fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageDefault::getFilenameCount() const
	{
		//неверно, есть еще CreateImage
		return m_vectorImageDescs.size();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::loader( XmlElement * _xml )
	{
		ResourceImage::loader( _xml );

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "File" )
			{
				ImageDesc desc;
				desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
				desc.offset = mt::vec2f(0.f,0.f);
				desc.maxSize = mt::vec2f(-1.f,-1.f);
				desc.size = mt::vec2f(-1.f,-1.f);
				desc.isAlpha = true; //
				desc.wrapX = false;
				desc.wrapY = false;

				String fileName; 

				String format;
				int from = -1;
				int to = -1;
				int step = 1;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", fileName );
					XML_CASE_ATTRIBUTE( "UV", desc.uv );
					XML_CASE_ATTRIBUTE( "Offset", desc.offset );
					XML_CASE_ATTRIBUTE( "MaxSize", desc.maxSize );
					XML_CASE_ATTRIBUTE( "Size", desc.size );
					XML_CASE_ATTRIBUTE( "Alpha", desc.isAlpha );

					XML_CASE_ATTRIBUTE( "From", from );
					XML_CASE_ATTRIBUTE( "To", to );
					XML_CASE_ATTRIBUTE( "Step", step );
					XML_CASE_ATTRIBUTE( "WrapX", desc.wrapX );
					XML_CASE_ATTRIBUTE( "WrapY", desc.wrapY );
				}

				if( from >= 0 && to >= 0 )
				{
					char* fname = new char[fileName.size() * 2];
					if( step > 0 )
					{
						for( int i = from; i <= to; i += step )
						{
							sprintf( fname, fileName.c_str(), i );

							desc.fileName = m_factoryIdentity->cacheIdentity( fname );
							m_vectorImageDescs.push_back( desc );
						}
					}
					else if( step < 0 )
					{
						for( int i = from; i >= to; i += step )
						{
							sprintf( fname, fileName.c_str(), i );

							desc.fileName = m_factoryIdentity->cacheIdentity( fname );
							m_vectorImageDescs.push_back( desc );
						}
					}
					delete[] fname;
				}
				else
				{
					desc.fileName = m_factoryIdentity->cacheIdentity( fileName );
					m_vectorImageDescs.push_back( desc );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		int i = 0;

		std::size_t createImageIdentity = m_factoryIdentity->cacheIdentity( "CreateImage" );
		std::size_t createTargetIdentity = m_factoryIdentity->cacheIdentity( "CreateTarget" );

		for( TVectorImageDesc::iterator
			it = m_vectorImageDescs.begin(),
			it_end = m_vectorImageDescs.end();
		it != it_end;
		++it)
		{
			ImageFrame frame;

			if( it->fileName == createImageIdentity )
			{
				const String & name = getName();

				String createImageName = name + Utils::toString( i++ );
				frame = createImageFrame( createImageName, it->size );
			}
			else if( it->fileName == createTargetIdentity )
			{
				const String & name = getName();

				frame = createRenderTargetFrame( name, it->size );
			}
			else
			{
				const String & category = this->getCategory();
				const String & fileName = m_factoryIdentity->getIdentity( it->fileName );

				frame = loadImageFrame( category, fileName );
			}

			if( frame.texture == NULL )
			{
				return false;
			}

			frame.uv = it->uv;
			
			frame.maxSize = it->maxSize;
			frame.offset =  it->offset;

			float ku = frame.uv.z - frame.uv.x;
			float kv = frame.uv.w - frame.uv.y;
			frame.size.x *= ku;
			frame.size.y *= kv;

			frame.size.x = ::floorf( frame.size.x + 0.5f );
			frame.size.y = ::floorf( frame.size.y + 0.5f );
			//mt::vec2f(frame.size.x * ku , frame.size.y * kv );

			if( frame.maxSize.x < 0.f || frame.maxSize.y < 0.f )
			{
				frame.maxSize = frame.size;
			}
			/*else
			{
				frame.size = it->size;
			}*/

			frame.isAlpha = it->isAlpha;
			frame.wrapX = it->wrapX;
			frame.wrapY = it->wrapY;

			m_vectorImageFrames.push_back( frame );
		}

		if( m_vectorImageFrames.empty() == true )
		{
			return false;
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
		desc.size = mt::vec2f(1.f,1.f);
		desc.isAlpha = false; //
		desc.fileName = m_factoryIdentity->cacheIdentity( _imagePath );

		m_vectorImageDescs.push_back( desc );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::getWrapX( std::size_t _frame ) const 
	{
		return m_vectorImageFrames[ _frame ].wrapX;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::getWrapY( std::size_t _frame ) const 
	{
		return m_vectorImageFrames[ _frame ].wrapY;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::setImagePath( const String& _imagePath )
	{
		if( m_vectorImageDescs.begin() != m_vectorImageDescs.end() )
		{
			m_vectorImageDescs[0].fileName = m_factoryIdentity->cacheIdentity( _imagePath );
		}
		else
		{
			addImagePath( _imagePath );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}
