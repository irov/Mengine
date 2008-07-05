#	include "AtlasTexture.h"
#	include "Utils.h"
#	include "FreeImageWrapper.h"
//////////////////////////////////////////////////////////////////////////
AtlasTexture::AtlasTexture(FILE * _log, int _width, int _height, int _bpp)
: m_atlasTexture(0)
, m_packWidth(_width)
, m_packHeight(_height)
, m_bpp(_bpp)
, m_log(_log)
{}
//////////////////////////////////////////////////////////////////////////
bool AtlasTexture::bake()
{
	m_atlasTexture = FreeImageWrapper::AllocateImage( m_packWidth, m_packHeight, m_bpp );

	FreeImageWrapper::FillChannel( m_atlasTexture, FI_RGBA_ALPHA, 0 );

	m_areaPacker.reset(m_packWidth, m_packHeight);

	return true;
}
//////////////////////////////////////////////////////////////////////////
int AtlasTexture::getWidth() const
{
	return FreeImage_GetWidth(m_atlasTexture);
}
//////////////////////////////////////////////////////////////////////////
int AtlasTexture::getHeight() const
{
	return FreeImage_GetHeight(m_atlasTexture);
}
//////////////////////////////////////////////////////////////////////////
int AtlasTexture::getBPP() const
{
	return FreeImage_GetBPP(m_atlasTexture);
}
//////////////////////////////////////////////////////////////////////////
bool AtlasTexture::isAlphaChannel() const
{
	return FreeImage_GetBPP(m_atlasTexture) == 32;
}
//////////////////////////////////////////////////////////////////////////
const std::string & AtlasTexture::getFilename() const
{
	return m_filename;
}
//////////////////////////////////////////////////////////////////////////
bool	AtlasTexture::isFitting(const Texture2D & _texture) const
{
	return ( _texture.getWidth() <= m_packWidth ) && ( _texture.getHeight() <= m_packHeight );
}
//////////////////////////////////////////////////////////////////////////
bool	AtlasTexture::insertTexture(Texture2D & _texture)
{
	int X = 0;
	int Y = 0;

	int Width = _texture.getWidth();
	int Height = _texture.getHeight();

	bool result = m_areaPacker.insert(Width,Height,X,Y);

	if(result == true)
	{
		_texture.setAtlas(this);

		bool res = FreeImage_Paste(m_atlasTexture,_texture.getTexture(),X,Y,256);

		_texture.m_textureDesc.X = X;
		_texture.m_textureDesc.Y = Y;

		_texture.m_textureDesc.u = float(_texture.m_textureDesc.X) / float(m_packWidth);
		_texture.m_textureDesc.v = float(_texture.m_textureDesc.Y) / float(m_packHeight);

		_texture.m_textureDesc.w = (_texture.m_textureDesc.X + _texture.getWidth()) / float(m_packWidth);
		_texture.m_textureDesc.z = (_texture.m_textureDesc.Y + _texture.getHeight()) / float(m_packWidth);
		
		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
void	AtlasTexture::writeToDisc(const std::string & _name)
{
	m_filename = _name;

	int width = m_areaPacker.getFilledWidth();
	int height = m_areaPacker.getFilledHeight();

	//uncomment for test
	//FreeImage_Save(FIF_PNG,m_atlasTexture,(_name+".png").c_str());

	FIBITMAP * rgb = FreeImage_ConvertTo24Bits(m_atlasTexture);

	DWORD size;
	BYTE * ourData = FreeImageWrapper::GrabMemory(rgb, FIF_JPEG, size, Utility::JPEGQuality);
	
	unsigned long isalpha = isAlphaChannel();

	FILE * f = fopen(m_filename.c_str(),"wb");

	fwrite(&isalpha,sizeof(unsigned long),1,f);
	fwrite(&size,sizeof(unsigned long),1,f);
	fwrite(ourData,sizeof(BYTE),size,f);
	
	delete[] ourData;

	FreeImage_Unload(rgb);

	if(isalpha == true)
	{
		FIBITMAP * alpha = FreeImage_GetChannel(m_atlasTexture,FICC_ALPHA);

		ourData = FreeImageWrapper::GrabMemory(alpha, FIF_PNG, size);

		fwrite(&size,sizeof(unsigned long),1,f);
		fwrite(ourData,sizeof(BYTE),size,f);
		fclose(f);

		delete[] ourData;

		FreeImage_Unload(alpha);
	}

	FreeImage_Unload(m_atlasTexture);
}