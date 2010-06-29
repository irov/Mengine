#	include "ResourceImageDynamic.h"

#	include "ResourceImplement.h"

#	include "RenderEngine.h"
#	include "FileEngine.h"
#	include "Texture.h"
#	include "Utils/Core/String.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceImageDynamic );
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDynamic::ResourceImageDynamic()
		: m_offset( 0.0f, 0.0f )
		, m_uv( 0.0f, 0.0f, 1.0f, 1.0f )
	{
		m_frame.texture = NULL;
		m_frame.size = mt::vec2f::zero_v2;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDynamic::~ResourceImageDynamic()
	{
		if( m_cachedPath.invalid() == false )
		{
			const ConstString & category = getCategory();

			FileEngine::get()
				->removeFile( category, m_cachedPath.str() );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getMaxSize( std::size_t _frame ) const
	{
		return m_frame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageDynamic::getCount() const
	{
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getSize( std::size_t _frame ) const
	{
		return m_frame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getOffset( std::size_t _frame ) const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDynamic::getUV( std::size_t _frame ) const
	{
		return m_uv;
	}
	/////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDynamic::getFilename( std::size_t _frame ) const
	{
		assert(!"ResourceImageDynamic::getFilename not implemented");
		return Utils::emptyConstString();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDynamic::getCodecType( std::size_t _frame ) const
	{
		assert(!"ResourceImageDynamic::getCodecType not implemented");
		return Utils::emptyConstString();
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceImageDynamic::getFilenameCount() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDynamic::isAlpha( std::size_t _frame ) const
	{
		//assert(!"ResourceImageDynamic::isAlpha not implemented");
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceImageDynamic::getTexture( std::size_t _frame )
	{
		return m_frame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::loader( XmlElement * _xml )
	{
		ResourceImage::loader( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::setRenderImage( Texture* _image )
	{
		m_frame.texture = _image;
		m_frame.size.x = (float)_image->getWidth();
		m_frame.size.y = (float)_image->getHeight();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDynamic::_compile()
	{	
		const ConstString & category = getCategory();

		if( m_cachedPath.invalid() )
		{
			const ConstString & group = getGroup();
			const ConstString & name = getName();			

			String cashName =  group.str() + "cache_" + name.str() + ".png";

			m_cachedPath = ConstManager::get()
				->genString( cashName );

			m_cachedCodec = ConstManager::get()
				->genString( "pngImage" );
		}

		m_frame = loadImageFrame_( category, m_cachedPath, m_cachedCodec );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::_release()
	{
		if( m_frame.texture != 0 )
		{
			const ConstString & category = getCategory();

			RenderEngine::get()
				->saveImage( m_frame.texture, category, m_cachedPath.str() );

			releaseImageFrame_( m_frame );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::setSize( const mt::vec2f& _size )
	{
		m_frame.size = _size;
	}
	//////////////////////////////////////////////////////////////////////////
}
