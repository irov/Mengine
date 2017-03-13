#	include "RenderTexture.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderTexture::RenderTexture()
        : m_id( 0 )
		, m_mipmaps(0)
		, m_width(0)
		, m_height(0)
        , m_channels(0)        
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTexture::~RenderTexture()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void RenderTexture::initialize( uint32_t _id
        , const RenderImageInterfacePtr & _image
		, uint32_t _mipmaps
        , uint32_t _width
        , uint32_t _height
        , uint32_t _channels        
        )
    {
        m_id = _id;

        m_image = _image;

		m_mipmaps = _mipmaps;
        m_width = _width;
        m_height = _height;
        m_channels = _channels;

        m_rect.left = 0;
        m_rect.top = 0;
        m_rect.right = m_width;
        m_rect.bottom = m_height;

        m_hwRect.left = 0;
        m_hwRect.top = 0;
        m_hwRect.right = m_image->getHWWidth();
        m_hwRect.bottom = m_image->getHWHeight();

		float scaleU = float(m_rect.right) / float(m_hwRect.right);
		float scaleV = float(m_rect.bottom) / float(m_hwRect.bottom);

		mt::uv4_from_mask( m_uv, mt::vec4f(0.f, 0.f, scaleU, scaleV) );

        m_pow2 = (m_hwRect.right == m_rect.right && m_hwRect.bottom == m_rect.bottom);
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
	uint32_t RenderTexture::getId() const
	{
		return m_id;
	}
    //////////////////////////////////////////////////////////////////////////
    bool RenderTexture::isPow2() const
    {
        return m_pow2;
    }
	//////////////////////////////////////////////////////////////////////////
	void RenderTexture::setCategory( const ConstString & _category )
	{
		m_category = _category;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderTexture::setFileName( const FilePath & _fileName )
	{
		m_fileName = _fileName;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t RenderTexture::getMipmaps() const
	{
		return m_mipmaps;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t RenderTexture::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t RenderTexture::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t RenderTexture::getChannels() const
	{
		return m_channels;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getMemoryUse() const
	{
		uint32_t HWWidth = m_image->getHWWidth();
		uint32_t HWHeight = m_image->getHWHeight();
		uint32_t HWChannels = m_image->getHWChannels();
		uint32_t HWBits = m_image->getHWDepth();

		PixelFormat HWFormat = m_image->getHWPixelFormat();

		size_t memroy_size = Helper::getTextureMemorySize( HWWidth, HWHeight, HWChannels, HWBits, HWFormat );

		return memroy_size;
	}
    /////////////////////////////////////////////////////////////////////////////
    Pointer RenderTexture::lock( size_t * _pitch, uint32_t _miplevel, const Rect& _rect, bool _readOnly /*= true */ ) const
	{
        Pointer buffer = m_image->lock( _pitch, _miplevel, _rect, _readOnly );

		return buffer;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderTexture::unlock( uint32_t _miplevel ) const
	{
		m_image->unlock( _miplevel );
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
	const mt::uv4f & RenderTexture::getUV() const
	{
		return m_uv;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
