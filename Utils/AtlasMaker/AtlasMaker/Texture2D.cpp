#	include "Texture2D.h"
#	include "AtlasTexture.h"
#	include "FreeImageWrapper.h"
//////////////////////////////////////////////////////////////////////////
Texture2D::Texture2D()
	: m_filename("")
	, m_texture(0)
	, m_atlas(0)
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
const Texture2D::TextureDesc& Texture2D::getDesc() const
{
	return m_textureDesc;
}
//////////////////////////////////////////////////////////////////////////
int Texture2D::getWidth()  const
{
	return FreeImage_GetWidth(m_texture);
}
//////////////////////////////////////////////////////////////////////////
int Texture2D::getHeight() const
{
	return FreeImage_GetHeight(m_texture);
}
//////////////////////////////////////////////////////////////////////////
int Texture2D::getBPP() const
{
	return FreeImage_GetBPP(m_texture);
}
//////////////////////////////////////////////////////////////////////////
bool Texture2D::isAlphaChannel() const
{
	return FreeImage_GetBPP(m_texture) == 32;
}
//////////////////////////////////////////////////////////////////////////
bool Texture2D::loadTexture( const std::string & _filename )
{
	m_filename = _filename;

	m_texture = FreeImage_Load(FIF_PNG,m_filename.c_str());

	if(m_texture == NULL)
	{
		return false;
	}

	_sliceAlpha();

	return true;
}
//////////////////////////////////////////////////////////////////////////
void Texture2D::_sliceAlpha()
{
	int width = FreeImage_GetWidth(m_texture);
	int height = FreeImage_GetHeight(m_texture);
	int pitch  = FreeImage_GetPitch(m_texture); 

	RECT imageRect = {0, 0, width, height};

	m_textureDesc.setRectImage(width, height);

	if(isAlphaChannel() == false)
	{
		return;
	}

	RECT boundingBox = {INT_MAX, INT_MAX, -1, -1};

	bool found = false;

	int bytespp = FreeImage_GetLine(m_texture) / FreeImage_GetWidth(m_texture); 
	  
    for(int y = FreeImage_GetHeight(m_texture) - 1; y >= 0; y--)
	{ 
		BYTE * bits = FreeImage_GetScanLine(m_texture, y); 
	  
		for(int x = 0; x < FreeImage_GetWidth(m_texture); x++)
		{ 
		  	if(bits[FI_RGBA_ALPHA] != 0)
			{
				int cy = FreeImage_GetHeight(m_texture) - 1 - y;

				if (cy < boundingBox.top) boundingBox.top = cy;
				if (cy > boundingBox.bottom) boundingBox.bottom = cy;

				if (x < boundingBox.left) boundingBox.left = x;
				if (x > boundingBox.right) boundingBox.right = x;

				found = true;
			}

			bits += bytespp; 
		} 
	}

	boundingBox.right++;
	boundingBox.bottom++;

	if(found == false)
	{
		boundingBox = imageRect;
	}

	m_textureDesc.offsetX = boundingBox.left;
	m_textureDesc.offsetY = boundingBox.top;

	m_textureDesc.sizeX = boundingBox.right - boundingBox.left;
	m_textureDesc.sizeY = boundingBox.bottom - boundingBox.top;

	FreeImageWrapper::CropImage(m_texture, boundingBox);
}
//////////////////////////////////////////////////////////////////////////