#	include "DX9Texture.h"

#	include "DX9ErrorHelper.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	DX9Texture::DX9Texture()
		: m_serviceProvider(nullptr)
		, m_d3dTexture(nullptr)
		, m_mode(ERIM_NORMAL)
        , m_hwWidth(0)
        , m_hwHeight(0)
        , m_hwChannels(0)
        , m_hwPixelFormat(PF_UNKNOWN)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	DX9Texture::~DX9Texture()
	{
        if( m_d3dTexture != nullptr )
        {			
            ULONG ref = m_d3dTexture->Release();
			(void)ref;

            m_d3dTexture = nullptr;
        }
	}
    //////////////////////////////////////////////////////////////////////////
    void DX9Texture::initialize( ServiceProviderInterface * _serviceProvider, IDirect3DTexture9 * _d3dInterface, ERenderImageMode _mode, size_t _hwWidth, size_t _hwHeight, size_t _hwChannels, PixelFormat _hwPixelFormat )
    {
		m_serviceProvider = _serviceProvider;
        m_d3dTexture = _d3dInterface;

		m_mode = _mode;

        m_hwWidth = _hwWidth;
        m_hwHeight = _hwHeight;
        m_hwChannels = _hwChannels;
        m_hwPixelFormat = _hwPixelFormat;
    }
	///////////////////////////////////////////////////////////////////////////
	void * DX9Texture::lock( int* _pitch, const Rect& _rect, bool _readOnly )
	{
		int flags;
		if( _readOnly == true )
		{
			flags = D3DLOCK_READONLY;
		}
		else 
		{
			flags = 0;
		}

		RECT rect;
		rect.top = _rect.top;
		rect.bottom = _rect.bottom;
		rect.left = _rect.left;
		rect.right = _rect.right;

		D3DLOCKED_RECT TRect;
		IF_DXCALL( m_serviceProvider, m_d3dTexture, LockRect, (0, &TRect, &rect, flags) )
		{
			return nullptr;
		}

		*_pitch = TRect.Pitch;

		void * bits = TRect.pBits;

		return bits;
	}
	//////////////////////////////////////////////////////////////////////////
	void DX9Texture::unlock()
	{
		DXCALL( m_serviceProvider, m_d3dTexture ,UnlockRect, (0) );
	}
	//////////////////////////////////////////////////////////////////////////
	IDirect3DTexture9 * DX9Texture::getDXTextureInterface() const
	{
		return m_d3dTexture;
	}
	//////////////////////////////////////////////////////////////////////////
	ERenderImageMode DX9Texture::getMode() const
	{
		return m_mode;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t DX9Texture::getHWWidth() const
	{
		return m_hwWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t DX9Texture::getHWHeight() const
	{
		return m_hwHeight;
	}
    //////////////////////////////////////////////////////////////////////////
    PixelFormat DX9Texture::getHWPixelFormat() const
    {
        return m_hwPixelFormat;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t DX9Texture::getHWChannels() const
    {
        return m_hwChannels;
    }
	//////////////////////////////////////////////////////////////////////////
	size_t DX9Texture::getHWDepth() const
	{
		return 1; //ToDo
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
