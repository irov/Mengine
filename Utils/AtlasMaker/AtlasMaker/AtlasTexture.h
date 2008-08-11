#	pragma once

#	include "Texture2D.h"
#	include "RectangleAreaPacker.h"
#	include <map>
#	include <list>

struct TextureAtlasDesc
{
	Texture2D * texture;

	int X;
	int Y;

	float u;
	float v;
	float w;
	float z;

	int offsetX;
	int offsetY;

	int maxSizeX;
	int maxSizeY;

	int sizeX;
	int sizeY;
};

class AtlasTexture
{
public:
	AtlasTexture(FILE * _log, int _width, int _height, int _bpp);
	~AtlasTexture();

public:
	int getBPP() const;
	int getWidth() const;
	int getHeight() const;
	bool isAlphaChannel() const;
	const std::string & getFilename() const;

public:
	const TextureAtlasDesc & getTextureDesc(const std::string & _filename);
	bool insertTexture( Texture2D & _texture );
	void writeToDisc( const std::string & _name );

private:

	bool _allocateAtlasTexture(int _width, int _height);

	typedef std::map<std::string, TextureAtlasDesc> MapTextureInfo;
	MapTextureInfo m_insertedTextures;

	RectangleAreaPacker m_areaPacker;
	std::string m_filename;

	FIBITMAP * m_atlasTexture;
	int m_packWidth;
	int m_packHeight;
	int m_bpp;
	FILE * m_log;
};