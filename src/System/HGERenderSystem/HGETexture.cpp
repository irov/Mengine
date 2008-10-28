
#	include "HGETexture.h"
#	include "Menge\PixelFormat.h"
//#	include "HGE.h"

//////////////////////////////////////////////////////////////////////////
HGETexture::HGETexture( HGE* _hge, bool _freeOnDelete )
: m_hge( _hge )
, m_hTexture( 0 )
, m_freeOnDelete( _freeOnDelete )
, m_ref( 0 )
, m_uvMask( 1.0f, 1.0f )
, m_pixelFormat( Menge::PF_A8R8G8B8 )
, m_width(0)
, m_height(0)
{

}
//////////////////////////////////////////////////////////////////////////
HGETexture::HGETexture( HGE* _hge, HTEXTURE _htex, const Menge::String& _name, std::size_t _width, std::size_t _height, bool _freeOnDelete )
: m_hge( _hge )
, m_hTexture( _htex )
, m_name( _name )
, m_freeOnDelete( _freeOnDelete )
, m_ref( 0 )
, m_uvMask( 1.0f, 1.0f )
, m_pixelFormat( Menge::PF_A8R8G8B8 )
, m_width( _width )
, m_height( _height )
{
	int hw = m_hge->Texture_GetWidth( m_hTexture );
	int hh = m_hge->Texture_GetHeight( m_hTexture );

	m_uvMask.x = float(m_width) / hw;
	m_uvMask.y = float(m_height) / hh;
}
//////////////////////////////////////////////////////////////////////////
HGETexture::HGETexture( HGE* _hge, const Menge::String& _name, std::size_t _width, std::size_t _height, bool _freeOnDelete )
: m_hge( _hge )
, m_name( _name )
, m_width( _width )
, m_height( _height )
, m_freeOnDelete( _freeOnDelete )
, m_ref( 0 )
, m_uvMask( 1.0f, 1.0f )
, m_pixelFormat( Menge::PF_A8R8G8B8 )
{
	m_hTexture = m_hge->Texture_Create( (int)_width, (int)_height );

	int hw = m_hge->Texture_GetWidth( m_hTexture );
	int hh = m_hge->Texture_GetHeight( m_hTexture );

	m_uvMask.x = float(m_width) / hw;
	m_uvMask.y = float(m_height) / hh;
}
//////////////////////////////////////////////////////////////////////////
HGETexture::~HGETexture()
{
	unload();
}
//////////////////////////////////////////////////////////////////////////
void HGETexture::load( const Menge::String& _name, std::size_t _width, std::size_t _height, const Menge::TextureDesc& _desc )
{
	m_name = _desc.name;
	m_hTexture = m_hge->Texture_Create( (int)_desc.width, (int)_desc.height );
	int hw = m_hge->Texture_GetWidth( m_hTexture );
	int hh = m_hge->Texture_GetHeight( m_hTexture );

	m_hge->Texture_LoadRawData( m_hTexture, static_cast<char*>( _desc.buffer ), 
		(int)_desc.size / (int)_desc.height, (int)_desc.width, (int)_desc.height, _desc.pixelFormat );

	m_width = _desc.width;
	m_height = _desc.height;

	m_pixelFormat = static_cast<Menge::PixelFormat>( _desc.pixelFormat );

	m_uvMask.x = float(m_width) / hw;
	m_uvMask.y = float(m_height) / hh;
}
//////////////////////////////////////////////////////////////////////////
void HGETexture::unload()
{
	if( m_hTexture && m_freeOnDelete )
	{
		m_hge->Texture_Free( m_hTexture );
		m_hTexture = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
std::size_t HGETexture::getWidth() const 
{
	return m_width;
}
//////////////////////////////////////////////////////////////////////////
std::size_t HGETexture::getHeight() const 
{
	return m_height;
}
//////////////////////////////////////////////////////////////////////////
const Menge::String & HGETexture::getDescription() const 
{
	return m_name;
}
//////////////////////////////////////////////////////////////////////////
HTEXTURE HGETexture::getHandle() const
{
	return m_hTexture;
}
//////////////////////////////////////////////////////////////////////////
const mt::vec2f& HGETexture::getUVMask() const
{
	return m_uvMask;
}
//////////////////////////////////////////////////////////////////////////
unsigned char* HGETexture::lock( int* _pitch, bool _readOnly )
{
	unsigned char* lockBlock = reinterpret_cast< unsigned char* >( m_hge->Texture_Lock( m_hTexture, _pitch, _readOnly, 0, 0, (int)m_width, (int)m_height ) );
	return lockBlock;
}
//////////////////////////////////////////////////////////////////////////
void HGETexture::unlock()
{
	m_hge->Texture_Unlock( m_hTexture );
}
//////////////////////////////////////////////////////////////////////////
Menge::PixelFormat HGETexture::getPixelFormat()
{
	return m_pixelFormat;
}
//////////////////////////////////////////////////////////////////////////
void HGETexture::restore( HTEXTURE _htex )
{
	m_hTexture = _htex;
}
//////////////////////////////////////////////////////////////////////////