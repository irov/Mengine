#	pragma once

#	include "Texture2D.h"
#	include "RectangleAreaPacker.h"

class AtlasTexture
{
public:
	AtlasTexture(FILE * _log, int _width, int _height, int _bpp);
public:
	bool bake();
public:
	bool isFitting(const Texture2D & _texture) const;
	bool insertTexture(Texture2D & _texture);
	void writeToDisc(const std::string & _name);
	const std::string & getFilename() const;
	int getBPP() const;
	int getWidth() const;
	int getHeight() const;
	bool isAlphaChannel() const;
private:
	RectangleAreaPacker m_areaPacker;
	std::string m_filename;
	FIBITMAP * m_atlasTexture;
	int m_packWidth;
	int m_packHeight;
	int m_bpp;
	FILE * m_log;
};