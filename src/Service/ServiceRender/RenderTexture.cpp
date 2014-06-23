#	include "RenderTexture.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderTexture::RenderTexture()
        : m_serviceProvider(nullptr)
		, m_width(0)
		, m_height(0)
        , m_channels(0)
		, m_uv(0.f, 0.f, 1.f, 1.f)
        , m_id(0)        
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTexture::~RenderTexture()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void RenderTexture::initialize( ServiceProviderInterface * _serviceProvider
        , const RenderImageInterfacePtr & _image
        , size_t _width
        , size_t _height
        , size_t _channels
        , size_t _id
        )
    {
        m_serviceProvider = _serviceProvider;

        m_image = _image;
        m_width = _width;
        m_height = _height;
        m_channels = _channels;
        m_id = _id;

        m_rect.left = 0;
        m_rect.top = 0;
        m_rect.right = m_width;
        m_rect.bottom = m_height;

        m_hwRect.left = 0;
        m_hwRect.top = 0;
        m_hwRect.right = m_image->getHWWidth();
        m_hwRect.bottom = m_image->getHWHeight();

		m_uv.x = 0.f;
		m_uv.y = 0.f;

		float scaleU = float(m_rect.right) / float(m_hwRect.right);
		float scaleV = float(m_rect.bottom) / float(m_hwRect.bottom);

		m_uv.z = scaleU;
		m_uv.w = scaleV;
    }
	//////////////////////////////////////////////////////////////////////////
	void RenderTexture::release()
	{
		m_image = nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterfacePtr & RenderTexture::getImage() const
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getId() const
	{
		return m_id;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderTexture::setFileName( const FilePath & _fileName )
	{
		m_fileName = _fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	const FilePath & RenderTexture::getFileName() const
	{
		return m_fileName;
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
		size_t HWWidth = m_image->getHWWidth();
		size_t HWHeight = m_image->getHWHeight();

		PixelFormat HWFormat = m_image->getHWPixelFormat();

		size_t memroy_size = Helper::getTextureMemorySize( HWWidth, HWHeight, 1, HWFormat );

		return memroy_size;
	}
    /////////////////////////////////////////////////////////////////////////////
	unsigned char * RenderTexture::lock( int* _pitch, const Rect& _rect, bool _readOnly /*= true */ ) const
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
