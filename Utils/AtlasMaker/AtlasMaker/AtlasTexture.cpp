#	include "AtlasTexture.h"
#	include "Utils.h"

AtlasTexture::AtlasTexture(int _width, int _height, int bpp)
: m_atlasTexture(0)
, m_packWidth(_width)
, m_packHeight(_height)
{
	m_atlasTexture = FreeImage_AllocateT(FIT_BITMAP,m_packWidth,m_packHeight);

	FIBITMAP * tmp = 0;

	if(bpp == 32)
	{
		tmp = FreeImage_ConvertTo32Bits(m_atlasTexture);
	}

	if(bpp == 24)
	{
		tmp = FreeImage_ConvertTo24Bits(m_atlasTexture);
	}

	if(bpp == 8)
	{
		tmp = FreeImage_ConvertTo8Bits(m_atlasTexture);
	}

	FreeImage_Unload(m_atlasTexture);

	m_atlasTexture = tmp;

	int bytespp = FreeImage_GetLine(m_atlasTexture) / FreeImage_GetWidth(m_atlasTexture); 
  
	for(unsigned int y = 0; y < FreeImage_GetHeight(m_atlasTexture); y++)
	{ 
		BYTE * bits = FreeImage_GetScanLine(m_atlasTexture, y); 

		for(unsigned int x = 0; x < FreeImage_GetWidth(m_atlasTexture); x++) 
		{ 
			bits[FI_RGBA_ALPHA] = 0; 
			bits += bytespp; 
		} 
	} 

	m_areaPacker.reset(m_packWidth, m_packHeight);
}

long AtlasTexture::getBPP() const
{
	return FreeImage_GetBPP(m_atlasTexture);
}

const std::string & AtlasTexture::getFilename() const
{
	return m_filename;
}

bool	AtlasTexture::isFitting(const MengeTexture2D & _texture) const
{
	return ( _texture.getWidth() <= m_packWidth ) && ( _texture.getHeight() <= m_packHeight );
}

bool	AtlasTexture::insertTexture(MengeTexture2D & _texture)
{
	int X = 0;
	int Y = 0;

	int Width = _texture.getWidth();
	int Height = _texture.getHeight();

	bool result = m_areaPacker.insert(Width,Height,X,Y);

	if(result == true)
	{
		_texture.setAtlas(this);

		bool res = FreeImage_Paste(m_atlasTexture,_texture.getTexture(),X,Y,255);

		_texture.m_textureDesc.X = X;
		_texture.m_textureDesc.Y = Y;

		_texture.m_textureDesc.u = float(_texture.m_textureDesc.X) / float(m_packWidth);
		_texture.m_textureDesc.v = float(_texture.m_textureDesc.Y) / float(m_packHeight);

		_texture.m_textureDesc.w = (float(_texture.m_textureDesc.X) + _texture.getWidth()) / float(m_packWidth);
		_texture.m_textureDesc.z = (float(_texture.m_textureDesc.Y) + _texture.getHeight()) / float(m_packWidth);

		if(Utility::useHalfTexel == true)
		{
			float uv_half_pixel_width = (0.5f / m_packWidth);
			float uv_half_pixel_height = (0.5f / m_packHeight);

			_texture.m_textureDesc.u += uv_half_pixel_width;
			_texture.m_textureDesc.v += uv_half_pixel_height;

			_texture.m_textureDesc.w -= uv_half_pixel_width;
			_texture.m_textureDesc.z -= uv_half_pixel_height;
		}
		
		return true;
	}

	return false;
}

BYTE * GrabMemory(FIBITMAP * bitmap, FREE_IMAGE_FORMAT type, DWORD & size, int flags = 0)
{
	FIMEMORY * memory = FreeImage_OpenMemory();

	bool result = FreeImage_SaveToMemory( type, bitmap, memory, flags );

	if(result == false)
	{
		return 0;
	}

	BYTE * data;

	FreeImage_AcquireMemory( memory, &data, &size );
	
	BYTE * ourData = new BYTE[size];
	memcpy( ourData, data, size );
	
	FreeImage_CloseMemory( memory );

	return ourData;
}

void	AtlasTexture::writeToDisc(const std::string & _name)
{
	std::string basename = _name.substr(0,_name.find_last_of("."));

	m_filename = basename + ".mne";

	FIBITMAP * rgb = FreeImage_ConvertTo24Bits(m_atlasTexture);

	DWORD size;
	BYTE * ourData = GrabMemory(rgb, FIF_JPEG, size, Utility::JPEGQuality);
	
	unsigned long isalpha = (getBPP() == 32);

	FILE * f = fopen(m_filename.c_str(),"wb");

	fwrite(&isalpha,sizeof(unsigned long),1,f);
	fwrite(&size,sizeof(unsigned long),1,f);
	fwrite(ourData,sizeof(BYTE),size,f);
	
	delete[] ourData;

	FreeImage_Unload(rgb);

	if(isalpha == true)
	{
		FIBITMAP * alpha = FreeImage_GetChannel(m_atlasTexture,FICC_ALPHA);

		ourData = GrabMemory(alpha, FIF_PNG, size);

		fwrite(&size,sizeof(unsigned long),1,f);
		fwrite(ourData,sizeof(BYTE),size,f);
		fclose(f);

		delete[] ourData;

		FreeImage_Unload(alpha);
	}

	FreeImage_Unload(m_atlasTexture);
}