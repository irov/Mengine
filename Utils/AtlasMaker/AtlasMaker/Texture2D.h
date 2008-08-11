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

public:
	void	setAtlas(AtlasTexture * _atlas);
	AtlasTexture * getAtlas() const;
	//void	_makeBorder(int _border);

	void	makeRigthBorder(int _border);
	void	makeBottomBorder(int _border);

public:
	int	getWidth() const;
	int	getHeight() const;
	int	getBPP() const;
	int getNonAlphaWidth()  const;
	int getNonAlphaHeight() const;
	int getOffsetX() const;
	int getOffsetY() const;
	int getBorder() const;

	bool isAlphaChannel() const;
	const	std::string & getFilename() const;

	FIBITMAP * getTexture() const;

private:

	int m_bottomBorder;
	int m_rigthBorder;

	int m_offsetX;
	int m_offsetY;

	int m_nonAlphaWidth;
	int m_nonAlphaHeight;

	int m_withAlphaWidth;
	int m_withAlphaHeight;

	int m_bpp;

	int m_border;

	FIBITMAP * m_texture;

	AtlasTexture * m_atlas;

	std::string m_filename;

	void	_sliceAlpha();
};