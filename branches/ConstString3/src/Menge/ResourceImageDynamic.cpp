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
		m_frame.size = mt::zero_v2;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImageDynamic::~ResourceImageDynamic()
	{
		if( m_cachedPath.empty() == false )
		{
			const ConstString & category = getCategory();

			FileEngine::get()
				->removeFile( category, Helper::to_str(m_cachedPath) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getMaxSize( size_t _frame ) const
	{
		return m_frame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageDynamic::getCount() const
	{
		return 1;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getSize( size_t _frame ) const
	{
		return m_frame.size;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & ResourceImageDynamic::getOffset( size_t _frame ) const
	{
		return m_offset;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDynamic::getUV( size_t _frame ) const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & ResourceImageDynamic::getUVImage( size_t _frame ) const
	{
		return m_uv;
	}
	/////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDynamic::getFileName( size_t _frame ) const
	{
		assert(!"ResourceImageDynamic::getFilename not implemented");
		return Utils::emptyConstString();
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & ResourceImageDynamic::getCodecType( size_t _frame ) const
	{
		assert(!"ResourceImageDynamic::getCodecType not implemented");
		return Utils::emptyConstString();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t ResourceImageDynamic::getFilenameCount() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceImageDynamic::isAlpha( size_t _frame ) const
	{
		//assert(!"ResourceImageDynamic::isAlpha not implemented");
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Texture* ResourceImageDynamic::getTexture( size_t _frame ) const
	{
		return m_frame.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::loader( BinParser * _parser )
	{
		ResourceImage::loader( _parser );
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

		if( m_cachedPath.empty() )
		{
			const ConstString & group = getGroup();
			const ConstString & name = getName();			

			String cashName =  Helper::to_str(group) + "cache_" + Helper::to_str(name) + ".png";

			m_cachedPath = ConstString(cashName);

			m_cachedCodec = ConstString("pngImage");
		}

		if( loadImageFrame_( m_frame, category, m_cachedPath, m_cachedCodec ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceImageDynamic::_release()
	{
		if( m_frame.texture != 0 )
		{
			const ConstString & category = getCategory();

			RenderEngine::get()
				->saveImage( m_frame.texture, category, Helper::to_str(m_cachedPath) );

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
