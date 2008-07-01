#	pragma once
#	include "MengeTexture.h"
#	include "RectangleAreaPacker.h"
#	include "FreeImage.h"

class AtlasTexture
{
public:
	AtlasTexture(int _width, int _height, int bpp);

public:
	bool isFitting(const MengeTexture2D & _texture) const;
	bool insertTexture(MengeTexture2D & _texture);
	void writeToDisc(const std::string & _name);
	const std::string & getFilename() const;
	long getBPP() const;
private:
	RectangleAreaPacker m_areaPacker;
	std::string m_filename;
	FIBITMAP * m_atlasTexture;
	int m_packWidth;
	int m_packHeight;
};