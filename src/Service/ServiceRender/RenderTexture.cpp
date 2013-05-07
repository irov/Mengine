#	include "RenderTexture.h"

#	include "Core/PixelFormat.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderTexture::RenderTexture()
		: m_width(0)
		, m_height(0)
        , m_channels(0)
		, m_uv(0.f, 0.f, 1.f, 1.f)
        , m_id(0)
        , m_listener(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTexture::~RenderTexture()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void RenderTexture::initialize( const RenderImageInterfacePtr & _image
        , size_t _width
        , size_t _height
        , size_t _channels
        , size_t _id
        , RenderTextureInterfaceListener * _listener )
    {
        m_image = _image;
        m_width = _width;
        m_height = _height;
        m_channels = _channels;
        m_id = _id;
        m_listener = _listener;

        m_rect.left = 0;
        m_rect.top = 0;
        m_rect.right = m_width;
        m_rect.bottom = m_height;

        m_hwRect.left = 0;
        m_hwRect.top = 0;
        m_hwRect.right = m_image->getHWWidth();
        m_hwRect.bottom = m_image->getHWHeight();
    }
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterfacePtr RenderTexture::getImage() const
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderTexture::_destroy()
	{
        m_listener->onRenderTextureRelease( this );

        m_image = nullptr;

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getId() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderTexture::setFileName( const FilePath & _filename )
	{
		m_filename = _filename;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & RenderTexture::getFileName() const
	{
		return m_filename;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getChannels() const
	{
		return m_channels;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getMemoryUse() const
	{
		size_t HWWidth = this->getHWWidth();
		size_t HWHeight = this->getHWHeight();

		PixelFormat HWFormat = this->getHWPixelFormat();

		size_t memroy_size = PixelUtil::getMemorySize( HWWidth, HWHeight, 1, HWFormat );

		return memroy_size;
	}
    /////////////////////////////////////////////////////////////////////////////
	unsigned char* RenderTexture::lock( int* _pitch, const Rect& _rect, bool _readOnly /*= true */ ) const
	{
        unsigned char * buffer = m_image->lock( _pitch, _rect, _readOnly );

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderTexture::unlock() const
	{
		m_image->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getHWWidth() const
	{
        size_t width = m_image->getHWWidth();

		return width;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getHWHeight() const
	{
        size_t height = m_image->getHWHeight();
        
		return height;
	}
    //////////////////////////////////////////////////////////////////////////
    size_t RenderTexture::getHWChannels() const
    {
        size_t channels = m_image->getHWChannels();

        return channels;
    }
	//////////////////////////////////////////////////////////////////////////
	Menge::PixelFormat RenderTexture::getHWPixelFormat() const
	{
        Menge::PixelFormat format = m_image->getHWPixelFormat();

		return format;
	}
	//////////////////////////////////////////////////////////////////////////
	const Rect & RenderTexture::getRect() const
	{
		return m_rect;
	}
	//////////////////////////////////////////////////////////////////////////
	const Rect & RenderTexture::getHWRect() const
	{
		return m_hwRect;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec4f & RenderTexture::getUV() const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
