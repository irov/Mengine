#	pragma once
#	include <vector>
#	include <map>
#	include <list>
#	include "RectangleAreaPacker.h"
#	include "MengeTexture.h"
#	include "AtlasTexture.h"

typedef std::vector<MengeTexture2D*> TTextureVector;
typedef std::map <long, TTextureVector> TNewFormatMap;
typedef std::list<AtlasTexture> TAtlasList;

class AtlasTextureContainer
{
public:
	AtlasTextureContainer(int _width, int _height);
	~AtlasTextureContainer();
public:

	const std::vector<std::string>& compileAtlas(
		std::map<std::string, MengeTexture2D*> & _textures, 
		const std::string& _outputFilename );

private:
	TAtlasList * m_atlases;
	int m_width;
	int m_height;

	void insertAtlas(int index, long bpp, const TTextureVector& textures);
};