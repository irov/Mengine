#	include "ResourceImageDefault.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"
#	include "ConstManager.h"
#	include "Utils/Core/String.h"

#	include "Consts.h"

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
	const ConstString & ResourceImageDefault::getFilename( std::size_t _frame ) const
	{
		return m_vectorImageDescs[ _frame ].fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDefault::getCodecType( std::size_t _frame ) const
	{
		return m_vectorImageDescs[ _frame ].codecType;
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
				ConstString codecType;

				String format;
				int from = -1;
				int to = -1;
				int step = 1;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Path", fileName );
					XML_CASE_ATTRIBUTE( "Codec", desc.codecType );
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

				if( desc.codecType.invalid() )
				{
					desc.codecType = s_getImageCodec( fileName );
				}

				if( from >= 0 && to >= 0 )
				{
					char* fname = new char[fileName.size() * 2];
					if( step > 0 )
					{
						for( int i = from; i <= to; i += step )
						{
							sprintf( fname, fileName.c_str(), i );

							desc.fileName = ConstManager::get()
								->genString( fname );

							m_vectorImageDescs.push_back( desc );
						}
					}
					else if( step < 0 )
					{
						for( int i = from; i >= to; i += step )
						{
							sprintf( fname, fileName.c_str(), i );

							desc.fileName = ConstManager::get()
								->genString( fname );

							m_vectorImageDescs.push_back( desc );
						}
					}
					delete[] fname;
				}
				else
				{
					desc.fileName = ConstManager::get()
						->genString( fileName );

					m_vectorImageDescs.push_back( desc );
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDefault::_compile()
	{	
		int i = 0;

		for( TVectorImageDesc::iterator
			it = m_vectorImageDescs.begin(),
			it_end = m_vectorImageDescs.end();
		it != it_end;
		++it)
		{
			ImageFrame frame;

			if( it->fileName == Consts::get()->c_CreateImage )
			{
				const ConstString & name = getName();

				String createImageName = name.str() + Utils::toString( i++ );

				ConstString fileName = ConstManager::get()
					->genString( createImageName );

				frame = createImageFrame_( fileName, it->size );
			}
			else if( it->fileName == Consts::get()->c_CreateTarget )
			{
				const ConstString & name = getName();

				frame = createRenderTargetFrame_( name, it->size );
			}
			else
			{
				const ConstString & category = this->getCategory();
				
				frame = loadImageFrame_( category, it->fileName, it->codecType );
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
			releaseImageFrame_( *it );
		}

		m_vectorImageFrames.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDefault::addImagePath( const ConstString& _imagePath )
	{
		ImageDesc desc;
		desc.uv = mt::vec4f(0.f,0.f,1.f,1.f);
		desc.offset = mt::vec2f(0.f,0.f);
		desc.maxSize = mt::vec2f(-1.f,-1.f);
		desc.size = mt::vec2f(1.f,1.f);
		desc.isAlpha = false; //
		desc.fileName = _imagePath;

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
}
