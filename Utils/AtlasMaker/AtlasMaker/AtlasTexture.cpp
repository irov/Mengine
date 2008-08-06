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
{
	m_areaPacker.reset(m_packWidth, m_packHeight);
}
//////////////////////////////////////////////////////////////////////////
AtlasTexture::~AtlasTexture()
{
	FreeImage_Unload(m_atlasTexture);
}
//////////////////////////////////////////////////////////////////////////
int AtlasTexture::getWidth() const
{
	return m_packWidth;
}
//////////////////////////////////////////////////////////////////////////
int AtlasTexture::getHeight() const
{
	return m_packHeight;
}
//////////////////////////////////////////////////////////////////////////
int AtlasTexture::getBPP() const
{
	return m_bpp;
}
//////////////////////////////////////////////////////////////////////////
bool AtlasTexture::isAlphaChannel() const
{
	return m_bpp == 32;
}
//////////////////////////////////////////////////////////////////////////
const std::string & AtlasTexture::getFilename() const
{
	return m_filename;
}
//////////////////////////////////////////////////////////////////////////
bool	AtlasTexture::insertTexture(Texture2D & _texture)
{
	int X = 0;
	int Y = 0;

	int Width = _texture.getWidth() + _texture.getBorder();
	int Height = _texture.getHeight() + _texture.getBorder();

	bool result = m_areaPacker.insert(Width,Height,X,Y);

	if(result == true)
	{
		_texture.setAtlas(this);

		TextureAtlasDesc desc;

		desc.texture = &_texture;
		desc.X = X;
		desc.Y = Y;
		desc.offsetX = _texture.getOffsetX();
		desc.offsetY = _texture.getOffsetY();
		desc.sizeX = _texture.getNonAlphaWidth();
		desc.sizeY = _texture.getNonAlphaHeight();
		desc.maxSizeX = _texture.getWidth();
		desc.maxSizeY = _texture.getHeight();

		m_insertedTextures.insert( std::make_pair( _texture.getFilename(), desc ) );
		
		return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
const TextureAtlasDesc & AtlasTexture::getTextureDesc(const std::string & _filename)
{
	MapTextureInfo::const_iterator it = m_insertedTextures.find(_filename);

	if(it == m_insertedTextures.end())
	{
		assert(!"ERRROROROOROR1!!");
	}

	return it->second;
}
//////////////////////////////////////////////////////////////////////////
void	AtlasTexture::writeToDisc( const std::string & _name )
{
	m_filename = _name;

	int correctedWidth = m_areaPacker.getMaxWidth();
	int correctedHeight = m_areaPacker.getMaxHeight();

	correctedWidth = FreeImageWrapper::getPow2Size(correctedWidth);
	correctedHeight = FreeImageWrapper::getPow2Size(correctedHeight);

	bool result = _allocateAtlasTexture(correctedWidth, correctedHeight);

	if(result == false)
	{
		fprintf(m_log, "Error: Creating Atlas texture failed! Size = [%d;%d] \n", correctedWidth, correctedHeight);
		return;
	}

	for(MapTextureInfo::iterator it = m_insertedTextures.begin(); 
		it != m_insertedTextures.end(); ++it)
	{
		TextureAtlasDesc & desc = it->second;

		Texture2D * texture = desc.texture;

		int X = desc.X;
		int Y = desc.Y;

		bool result = FreeImage_Paste(m_atlasTexture, texture->getTexture(), X, Y, 256);

		if(result == false)
		{
			assert(!"ERROR!!!");
		}

		desc.u = X / float(correctedWidth);
		desc.v = Y / float(correctedHeight);

		desc.w = (X + desc.sizeX) / float(correctedWidth);
		desc.z = (Y + desc.sizeY) / float(correctedHeight);
	}

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
}
//////////////////////////////////////////////////////////////////////////
bool AtlasTexture::_allocateAtlasTexture(int _width, int _height)
{
	m_packWidth = _width;
	m_packHeight = _height;

	m_atlasTexture = FreeImageWrapper::AllocateImage( m_packWidth, m_packHeight, m_bpp );

	if(m_atlasTexture == NULL)
	{
		return false;
	}

	FreeImageWrapper::FillChannel( m_atlasTexture, FI_RGBA_ALPHA, 0 );

	return true;
}
//////////////////////////////////////////////////////////////////////////