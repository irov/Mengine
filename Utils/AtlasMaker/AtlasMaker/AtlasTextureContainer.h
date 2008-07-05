#	pragma once
#	include <vector>
#	include <map>
#	include <list>
#	include "RectangleAreaPacker.h"
#	include "Texture2D.h"
#	include "AtlasTexture.h"

typedef std::vector<Texture2D*> TTextureVector;
typedef std::map <int, TTextureVector> TNewFormatMap;
typedef std::list<AtlasTexture> TAtlasList;

class AtlasTextureContainer
{
public:
	AtlasTextureContainer(FILE * _log, int _width, int _height);
	~AtlasTextureContainer();
public:
	const std::vector<std::string>& compileAtlas(
		std::map<std::string, Texture2D*> & _textures, 
		const std::string& _outputFilename );

private:
	FILE * m_log;
	TAtlasList * m_atlases;
	int m_width;
	int m_height;

	void _insertAtlas(int index, int bpp, const TTextureVector& textures);
};