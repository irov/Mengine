#	include "RenderTexture.h"

#	include "Core/PixelFormat.h"

#	include "Logger/Logger.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RenderTexture::RenderTexture( RenderImageInterface* _image
						, size_t _width
						, size_t _height
                        , size_t _channels
						, size_t _id )
		: m_image(_image)
		, m_width(_width)
		, m_height(_height)
        , m_channels(_channels)
		, m_ref(1)
		, m_uv(0.f, 0.f, 1.f, 1.f)
		, m_id(_id)
	{
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
	RenderTexture::~RenderTexture()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderImageInterface* RenderTexture::getImage() const
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderTexture::destroy()
	{
		m_image->destroy();
        m_image = NULL;

        delete this;
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
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::addRef() const
	{        
		return ++m_ref;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::decRef() const
	{
		return --m_ref;
	}
    /////////////////////////////////////////////////////////////////////////////
	unsigned char* RenderTexture::lock( int* _pitch, const Rect& _rect, bool _readOnly /*= true */ ) const
	{
		return m_image->lock( _pitch, _rect, _readOnly );
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderTexture::unlock() const
	{
		m_image->unlock();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getHWWidth() const
	{
		return m_image->getHWWidth();
	}
	//////////////////////////////////////////////////////////////////////////
	size_t RenderTexture::getHWHeight() const
	{
		return m_image->getHWHeight();
	}
    //////////////////////////////////////////////////////////////////////////
    size_t RenderTexture::getHWChannels() const
    {
        return m_image->getHWChannels();
    }
	//////////////////////////////////////////////////////////////////////////
	Menge::PixelFormat RenderTexture::getHWPixelFormat() const
	{
		return m_image->getHWPixelFormat();
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
