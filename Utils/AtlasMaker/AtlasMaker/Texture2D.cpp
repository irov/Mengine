#	include "Texture2D.h"
#	include "FreeImageWrapper.h"
//////////////////////////////////////////////////////////////////////////
Texture2D::Texture2D()
	: m_filename("")
	, m_texture(0)
	, m_atlas(0)
	, m_offsetX(0)
	, m_offsetY(0)
	, m_nonAlphaWidth(0)
	, m_nonAlphaHeight(0)
	, m_withAlphaWidth(0)
	, m_withAlphaHeight(0)
	, m_bpp(0)
{}
//////////////////////////////////////////////////////////////////////////
Texture2D::~Texture2D()
{
	FreeImage_Unload(m_texture);
}
//////////////////////////////////////////////////////////////////////////
FIBITMAP * Texture2D::getTexture() const
{
	return m_texture;
}
//////////////////////////////////////////////////////////////////////////
void Texture2D::setAtlas(AtlasTexture * _atlas)
{
	m_atlas = _atlas;
}
//////////////////////////////////////////////////////////////////////////
AtlasTexture * Texture2D::getAtlas() const
{
	return m_atlas;
}
//////////////////////////////////////////////////////////////////////////
const std::string & Texture2D::getFilename() const
{
	return m_filename;
}
//////////////////////////////////////////////////////////////////////////
int Texture2D::getWidth()  const
{
	return m_withAlphaWidth;
}
//////////////////////////////////////////////////////////////////////////
int Texture2D::getHeight() const
{
	return m_withAlphaHeight;
}
//////////////////////////////////////////////////////////////////////////
int Texture2D::getNonAlphaWidth()  const
{
	return m_nonAlphaWidth;
}
//////////////////////////////////////////////////////////////////////////
int Texture2D::getNonAlphaHeight() const
{
	return m_nonAlphaHeight;
}
//////////////////////////////////////////////////////////////////////////
int Texture2D::getOffsetX() const
{
	return m_offsetX;
}
//////////////////////////////////////////////////////////////////////////
int Texture2D::getOffsetY() const
{
	return m_offsetY;
}
//////////////////////////////////////////////////////////////////////////
int Texture2D::getBPP() const
{
	return m_bpp;
}
//////////////////////////////////////////////////////////////////////////
bool Texture2D::isAlphaChannel() const
{
	return m_bpp == 32;
}
//////////////////////////////////////////////////////////////////////////
bool Texture2D::loadTexture( const std::string & _filename )
{
	m_filename = _filename;

	m_texture = FreeImage_Load( FIF_PNG, m_filename.c_str() );

	if( m_texture == NULL )
	{
		return false;
	}

	m_bpp = FreeImage_GetBPP(m_texture);

	m_withAlphaWidth = FreeImage_GetWidth(m_texture);
	m_withAlphaHeight = FreeImage_GetHeight(m_texture);

	m_nonAlphaWidth = m_withAlphaWidth;
	m_nonAlphaHeight = m_withAlphaHeight;

	_sliceAlpha();

	//_makeBorder(1);

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Texture2D::_sliceAlpha()
{
	RECT bbox;

	bool result = FreeImageWrapper::FindAlphaBoundingBox( m_texture, bbox );

	if( result == false )
	{
		return;
	}

	m_offsetX = bbox.left;
	m_offsetY = bbox.top;

	m_nonAlphaWidth = bbox.right - bbox.left;
	m_nonAlphaHeight = bbox.bottom - bbox.top;

	FreeImageWrapper::CropImage( m_texture, bbox );
}
//////////////////////////////////////////////////////////////////////////