#	pragma once
#	include <d3dx9.h>
#	include <assert.h>
#	include <string>
#	include "FreeImage.h"

class AtlasTexture;

class Texture2D
{
public:

	struct TextureDesc
	{
		TextureDesc()
			: X(0)
			, Y(0)
			, offsetX(0)
			, offsetY(0)
			, maxSizeX(0)
			, maxSizeY(0)
			, sizeX(0)
			, sizeY(0)
			, u(0.f)
			, v(0.f)
			, w(1.f)
			, z(1.f)
		{}

		int X;
		int Y;

		int offsetX;
		int offsetY;

		int maxSizeX;
		int maxSizeY;

		int sizeX;
		int sizeY;

		float u;
		float v;
		float w;
		float z;

		void setRectImage(int width, int height)
		{
			X = 0; 
			Y = 0;
			offsetX = 0; 
			offsetY = 0;
			maxSizeX = width;
			maxSizeY = height;
			sizeX = width;
			sizeY = height;
			u = 0.f;
			v = 0.f;
			w = 1.f;
			z = 1.f;
		}
	};

public:

	Texture2D();
	~Texture2D();

public:
	void setAtlas(AtlasTexture * _atlas);
	AtlasTexture * getAtlas() const;
	const	TextureDesc& getDesc() const;

public:

	bool	loadTexture(const std::string & _filename);
	const	std::string & getFilename() const;

public:
	int	getWidth() const;
	int	getHeight() const;
	int	getBPP() const;
	bool isAlphaChannel() const;
	FIBITMAP * getTexture() const;

private:

	FIBITMAP * m_texture;
	TextureDesc m_textureDesc;
	AtlasTexture * m_atlas;
	std::string m_filename;

	void	_sliceAlpha();

	friend class AtlasTexture;
};