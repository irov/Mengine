#	include "RenderSubTexture.h"

#	include "Interface/ImageCodecInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderSubTexture::RenderSubTexture()
		: m_listener(nullptr)
		, m_id(0)
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	RenderSubTexture::~RenderSubTexture()
	{		
	}
    //////////////////////////////////////////////////////////////////////////
    void RenderSubTexture::initialize( const RenderTextureInterfacePtr & _texture
        , const Rect & _rect
        , size_t _id
        , RenderTextureInterfaceListener * _listener )
    {
        m_texture = _texture;
        m_hwRect = _rect;
        m_listener = _listener;
        m_id = _id;

        m_rect.left = 0;
        m_rect.top = 0;
        m_rect.right = m_hwRect.getWidth();
        m_rect.bottom = m_hwRect.getHeight();

        size_t texture_width = m_texture->getWidth();
        size_t texture_height = m_texture->getHeight();

        m_uv.x = float(m_hwRect.left) / float(texture_width);
        m_uv.y = float(m_hwRect.top) / float(texture_height);
        m_uv.z = float(m_hwRect.right) / float(texture_width);
        m_uv.w = float(m_hwRect.bottom) / float(texture_height);
    }
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterfacePtr & RenderSubTexture::getImage() const
	{
        const RenderImageInterfacePtr & image = m_texture->getImage();

		return image;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::getId() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	const Rect & RenderSubTexture::getRect() const
	{
		return m_rect;
	}
	//////////////////////////////////////////////////////////////////////////
	const Rect & RenderSubTexture::getHWRect() const
	{
		return m_hwRect;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & RenderSubTexture::getUV() const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderSubTexture::setFileName( const FilePath & _fileName )
	{
		m_fileName = _fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & RenderSubTexture::getFileName() const
	{
		return m_fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::getWidth() const
	{
        size_t width = m_rect.getWidth();

		return width;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::getHeight() const
	{
        size_t height = m_rect.getHeight();

		return height;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::getChannels() const
	{
        size_t channels = m_texture->getChannels();

		return channels;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderSubTexture::getMemoryUse() const
	{
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned char * RenderSubTexture::lock( int* _pitch, const Rect& _rect, bool _readOnly ) const
	{
		Rect rect;

		rect.left = m_hwRect.left + _rect.left;
		rect.top = m_hwRect.top + _rect.top;
		rect.right = m_hwRect.left + _rect.right;
		rect.bottom = m_hwRect.top + _rect.bottom;

        unsigned char * buffer = m_texture->lock( _pitch, rect, _readOnly );

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderSubTexture::unlock() const
	{
		m_texture->unlock();
	}
}	// namespace Menge
