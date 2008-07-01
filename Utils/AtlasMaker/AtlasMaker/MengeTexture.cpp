#	include "MengeTexture.h"
#	include "AtlasTexture.h"
#	include "Utils.h"

MengeTexture2D::MengeTexture2D()
	: m_filename("")
	, m_texture(0)
	, m_atlas(0)
	, m_isAlphaChannelSupport(false)
{}

MengeTexture2D::~MengeTexture2D()
{
	FreeImage_Unload(m_texture);
}

FIBITMAP * MengeTexture2D::getTexture() const
{
	return m_texture;
}

void MengeTexture2D::setAtlas(AtlasTexture * _atlas)
{
	m_atlas = _atlas;
}

AtlasTexture * MengeTexture2D::getAtlas() const
{
	return m_atlas;
}

const std::string & MengeTexture2D::getFilename() const
{
	return m_filename;
}

const MengeTexture2D::TextureDesc& MengeTexture2D::getDesc() const
{
	return m_textureDesc;
}

long MengeTexture2D::getWidth()  const
{
	return FreeImage_GetWidth(m_texture);
}

long MengeTexture2D::getHeight() const
{
	return FreeImage_GetHeight(m_texture);
}

long MengeTexture2D::getBPP() const
{
	return FreeImage_GetBPP(m_texture);
}

bool	MengeTexture2D::isAlphaChannel() const
{
	return m_isAlphaChannelSupport;
}

bool MengeTexture2D::loadTexture(const std::string & _filename)
{
	m_filename = _filename;

	m_texture = FreeImage_Load(FIF_PNG,m_filename.c_str());

	if(m_texture == NULL)
	{
		return false;
	}

	sliceAlpha();

	return true;
}

void MengeTexture2D::sliceAlpha()
{
	unsigned int width = FreeImage_GetWidth(m_texture);
	unsigned int height = FreeImage_GetHeight(m_texture);
	unsigned int pitch  = FreeImage_GetPitch(m_texture); 

	m_textureDesc.sizeX = width;
	m_textureDesc.sizeY = height;

	m_textureDesc.maxSizeX = width;
	m_textureDesc.maxSizeY = height;

	if(FreeImage_GetBPP(m_texture) < 32)
	{
		m_isAlphaChannelSupport = false;
		return;
	}

	m_isAlphaChannelSupport = true;

	int minX = INT_MAX; 
	int minY = INT_MAX;

	int maxX = -1; 
	int maxY = -1;

	bool found = false;

	int bytespp = FreeImage_GetLine(m_texture) / FreeImage_GetWidth(m_texture); 
	  
    for(int y = FreeImage_GetHeight(m_texture)-1; y >= 0; y--)
	{ 
		BYTE *bits = FreeImage_GetScanLine(m_texture, y); 
	 
		for(int x = 0; x < FreeImage_GetWidth(m_texture); x++)
		{ 
		  	if(bits[FI_RGBA_ALPHA] != 0)
			{
				int cy = FreeImage_GetHeight(m_texture)-1 - y;

				if (cy < minY) minY = cy;
				if (cy > maxY) maxY = cy;

				if (x < minX) minX = x;
				if (x > maxX) maxX = x;

				found = true;
			}

			bits += bytespp; 
		} 
	}

	maxX++;
	maxY++;

	if(found == false)
	{
		minX = 0; 
		minY = 0;
		maxX = width; 
		maxY = height;
	}

	m_textureDesc.sizeX = maxX - minX;
	m_textureDesc.sizeY = maxY - minY;

	m_textureDesc.offsetX = minX;
	m_textureDesc.offsetY = minY;

	FIBITMAP * clampedTexture = FreeImage_Copy(m_texture,minX,minY,maxX,maxY);

	FreeImage_Unload(m_texture);

	m_texture = clampedTexture;
}