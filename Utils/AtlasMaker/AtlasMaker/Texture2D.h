#	pragma once
#	include <d3dx9.h>
#	include <assert.h>
#	include <string>
#	include "FreeImage.h"

class AtlasTexture;

class Texture2D
{

public:

	Texture2D();
	~Texture2D();

public:

	bool	loadTexture(const std::string & _filename);
	const	std::string & getFilename() const;

public:
	void setAtlas(AtlasTexture * _atlas);
	AtlasTexture * getAtlas() const;

public:
	int	getWidth() const;
	int	getHeight() const;
	int	getBPP() const;

	bool isAlphaChannel() const;

	int getNonAlphaWidth()  const;
	int getNonAlphaHeight() const;
	int getOffsetX() const;
	int getOffsetY() const;

	FIBITMAP * getTexture() const;

private:

	int m_offsetX;
	int m_offsetY;

	int m_nonAlphaWidth;
	int m_nonAlphaHeight;

	int m_withAlphaWidth;
	int m_withAlphaHeight;

	FIBITMAP * m_texture;

	AtlasTexture * m_atlas;

	std::string m_filename;

	void	_sliceAlpha();
};