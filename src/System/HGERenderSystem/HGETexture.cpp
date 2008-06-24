
#	include "HGETexture.h"
//#	include "HGE.h"

//////////////////////////////////////////////////////////////////////////
HGETexture::HGETexture( HGE* _hge, bool _freeOnDelete )
: m_hge( _hge )
, m_hTexture( 0 )
, m_freeOnDelete( _freeOnDelete )
, m_ref( 0 )
{

}
//////////////////////////////////////////////////////////////////////////
HGETexture::HGETexture( HGE* _hge, HTEXTURE _htex, const Menge::String& _name, bool _freeOnDelete )
: m_hge( _hge )
, m_hTexture( _htex )
, m_name( _name )
, m_freeOnDelete( _freeOnDelete )
, m_ref( 0 )
{

}
//////////////////////////////////////////////////////////////////////////
HGETexture::HGETexture( HGE* _hge, const Menge::String& _name, std::size_t _width, std::size_t _height, bool _freeOnDelete )
: m_hge( _hge )
, m_name( _name )
, m_width( _width )
, m_height( _height )
, m_freeOnDelete( _freeOnDelete )
, m_ref( 0 )
{
	m_hTexture = m_hge->Texture_Create( _width, _height );
}
//////////////////////////////////////////////////////////////////////////
HGETexture::~HGETexture()
{
	unload();
}
//////////////////////////////////////////////////////////////////////////
void HGETexture::load( const TextureDesc& _desc )
{
	m_name = _desc.name;
	m_hTexture = m_hge->Texture_Create( _desc.width, _desc.height );
	int hw = m_hge->Texture_GetWidth( m_hTexture );
	int hh = m_hge->Texture_GetHeight( m_hTexture );
	m_hge->Texture_LoadRawData( m_hTexture, static_cast<char*>( _desc.buffer ), 
		_desc.size / _desc.height, _desc.width, _desc.height, _desc.pixelFormat );

	m_width = _desc.width;
	m_height = _desc.height;
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
float HGETexture::getWidth() const 
{
	return m_width;
}
//////////////////////////////////////////////////////////////////////////
float HGETexture::getHeight() const 
{
	return m_height;
}
//////////////////////////////////////////////////////////////////////////
void HGETexture::writeToFile( const char* _filename )
{
	m_hge->Texture_WriteToFile( m_hTexture, _filename );
}
//////////////////////////////////////////////////////////////////////////
const char * HGETexture::getDescription() const 
{
	return m_name.c_str();
}
//////////////////////////////////////////////////////////////////////////
unsigned long HGETexture::getHandle() const
{
	return m_hTexture;
}
//////////////////////////////////////////////////////////////////////////