#	include "AtlasTexture.h"
#	include "Utils.h"
#	include "FreeImageWrapper.h"
#	include <functional>
#	include <stdio.h>
//////////////////////////////////////////////////////////////////////////
AtlasTexture::AtlasTexture(FILE * _log, int _width, int _height, int _bpp)
: m_atlasTexture(0)
, m_log(_log)
, m_packWidth(_width)
, m_packHeight(_height)
, m_bpp(_bpp)
{
	m_areaPacker.reset(m_packWidth, m_packHeight);
}
//////////////////////////////////////////////////////////////////////////
AtlasTexture::~AtlasTexture()
{
	m_atlasTexture = NULL;
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

	int Width = 0;
	int Height = 0;

	Width = _texture.getNonAlphaWidth();
	Height = _texture.getNonAlphaHeight();

	if(_texture.getNonAlphaWidth() == getWidth())
	{
	}
	else if(_texture.getNonAlphaWidth() < getWidth())
	{
		Width++;
	}
	else
	{
		return false;
	}
	
	if(_texture.getNonAlphaHeight() == getHeight())
	{
	}
	else if(_texture.getNonAlphaHeight() < getHeight())
	{
		Height++;
	}
	else
	{
		return false;
	}

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
		fprintf(m_log, "Error: %s getTextureDesc failed\n", _filename.c_str());
		//assert(!"ERRROROROOROR1!!");
	}

	return it->second;
}
class TextureAtlasCache
{
public:

	TextureAtlasCache()
	{

	}

	TextureAtlasCache(int width, int height,int bpp)
	{
		int bpps[] = {8,24,32};
		for(int k = 0; k < sizeof(bpps)/sizeof(int); k++)
		{
			for(int i = 1; i <= width; i <<= 1)
			{
				for(int j = 1; j <= height; j <<= 1)
				{
					FIBITMAP * m_atlasTexture = FreeImageWrapper::AllocateImage( i, j, bpps[k] );

					if(m_atlasTexture == NULL)
					{
						break;
					}

					//FreeImageWrapper::FillChannel( m_atlasTexture, FI_RGBA_ALPHA, 0 );

					//Desc desc(i,j,bpps[k]);
					static char Text[128];
					sprintf(Text,"%d %d %d",i,j,bpps[k]);
					std::string id = Text;
					CacheAtlases.insert(std::make_pair(id, m_atlasTexture));
				}
			}
		}
	}

	FIBITMAP* find(int width, int height,int bpp)
	{
		static char Text[128];
		sprintf(Text, "%d %d %d",width,height,bpp);
		std::string id = Text;

		std::map<std::string, FIBITMAP*>::iterator it = CacheAtlases.find(id);

		if(it == CacheAtlases.end())
		{
			FIBITMAP * m_atlasTexture = FreeImageWrapper::AllocateImage( width, height, bpp );

			if(m_atlasTexture == NULL)
			{
				return NULL;
			}

			static char Text[128];
			sprintf(Text,"%d %d %d",width, height, bpp);
			std::string id = Text;
			CacheAtlases.insert(std::make_pair(id, m_atlasTexture));
			return m_atlasTexture;
		}

		return it->second;
	}

	/*	struct Desc
	{
	int width;
	int height;
	int bpp;

	Desc(int i, int j, int k)
	: width(i)
	, height(j)
	, bpp(k)
	{
	}
	};

	struct less_desc
	: public std::binary_function<Desc, Desc, bool>
	{
	bool operator()(const Desc& desc1, const Desc& desc2) const
	{	
	return (desc1.width < desc2.width) && (desc1.height < desc2.height) && (desc1.bpp < desc2.bpp);
	}
	};
	*/
	std::map<std::string, FIBITMAP*> CacheAtlases;
};
//////////////////////////////////////////////////////////////////////////
void	AtlasTexture::writeToDisc( const std::string & _name )
{
	m_filename = _name;
	
	int correctedWidth = m_areaPacker.getMaxWidth();
	int correctedHeight = m_areaPacker.getMaxHeight();

	fprintf(m_log, "Process: Atlas %s non corrected size[%d;%d] writing to disc\n", m_filename.c_str(), correctedWidth, correctedHeight);

	correctedWidth = FreeImageWrapper::getPow2Size(correctedWidth);
	correctedHeight = FreeImageWrapper::getPow2Size(correctedHeight);

	fprintf(m_log, "Atlas %s [%d;%d] writing to disc\n", m_filename.c_str(), correctedWidth, correctedHeight);

	static TextureAtlasCache atlasCache;//(2048,2048,32);

	//сделать КЕШ атласов.
	//bool result = _allocateAtlasTexture(correctedWidth, correctedHeight);

	m_packWidth = correctedWidth;
	m_packHeight = correctedHeight;

	m_atlasTexture = atlasCache.find(m_packWidth,m_packHeight,m_bpp);

	//m_atlasTexture = FreeImageWrapper::AllocateImage( m_packWidth, m_packHeight, m_bpp );

	if(m_atlasTexture == NULL)
	{
		fprintf(m_log, "Error: Creating Atlas texture failed! Size = [%d;%d] \n", correctedWidth, correctedHeight);
		return;
	}

	FreeImageWrapper::FillChannel( m_atlasTexture, FI_RGBA_ALPHA, 0 );

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
			fprintf(m_log, "Error: Atlas %s FreeImage_Paste failed\n", _name.c_str());
			//assert(!"ERROR!!!");
		}

		desc.u = X / float(correctedWidth);
		desc.v = Y / float(correctedHeight);

		desc.w = (X + desc.sizeX) / float(correctedWidth);
		desc.z = (Y + desc.sizeY) / float(correctedHeight);
	}

	printf("%s \n",m_filename.c_str() );
	//uncomment for test
	//FreeImage_Save(FIF_PNG,m_atlasTexture,(m_filename+".png").c_str());

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
		
		delete[] ourData;

		FreeImage_Unload(alpha);
	}

	fclose(f);
}
//////////////////////////////////////////////////////////////////////////
/*bool AtlasTexture::_allocateAtlasTexture(int _width, int _height)
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
}*/
//////////////////////////////////////////////////////////////////////////